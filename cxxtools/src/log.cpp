/*
 * Copyright (C) 2012 Tommi Maekitalo
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <cxxtools/log/cxxtools.h>
#include <cxxtools/refcounted.h>
#include <cxxtools/smartptr.h>
#include <cxxtools/convert.h>
#include <cxxtools/mutex.h>
#include <cxxtools/atomicity.h>
#include <cxxtools/serializationinfo.h>
#include <cxxtools/xml/xmldeserializer.h>
#include <cxxtools/net/udp.h>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>

namespace cxxtools
{
  namespace
  {
    Mutex loggersMutex;
    Mutex logMutex;
    Mutex poolMutex;
    atomic_t mutexWaitCount = 0;

    template <typename T, unsigned MaxPoolSize = 8>
    class LPool
    {
        std::vector<T*> pool;
        Mutex mutex;

      public:
        ~LPool()
        {
          for (unsigned n = 0; n < pool.size(); ++n)
            delete pool[n];
        }

        T* getInstance()
        {
          if (pool.empty())
          {
            // we don't care about locking here since it is not dangerous to get a false answer
            return new T();
          }

          T* impl;

          {
            MutexLock lock(poolMutex);
            if (pool.empty())
              return new T();

            impl = pool.back();
            pool.pop_back();
          }

          return impl;
        }

        void releaseInstance(T* inst)
        {
          MutexLock lock(poolMutex);

          if (pool.size() < MaxPoolSize)
            pool.push_back(inst);
          else
            delete inst;
        }

    };

    class ScopedAtomicIncrementer
    {
        atomic_t& count;

      public:
        explicit ScopedAtomicIncrementer(atomic_t& count_)
          : count(count_)
        {
          atomicIncrement(count);
        }

        ~ScopedAtomicIncrementer()
        {
          atomicDecrement(count);
        }
    };

    void logentry(std::string& entry, const char* level, const std::string& category)
    {
      struct timeval t;
      gettimeofday(&t, 0);

      // format date only once per second:
      static char date[20];
      static time_t psec = 0;
      time_t sec = static_cast<time_t>(t.tv_sec);
      if (sec != psec)
      {
        struct tm tt;
        localtime_r(&sec, &tt);
        int year = 1900 + tt.tm_year;
        int mon = tt.tm_mon + 1;
        date[0] = static_cast<char>('0' + year / 1000 % 10);
        date[1] = static_cast<char>('0' + year / 100 % 10);
        date[2] = static_cast<char>('0' + year / 10 % 10);
        date[3] = static_cast<char>('0' + year % 10);
        date[4] = '-';
        date[5] = static_cast<char>('0' + mon / 10);
        date[6] = static_cast<char>('0' + mon % 10);
        date[7] = '-';
        date[8] = static_cast<char>('0' + tt.tm_mday / 10);
        date[9] = static_cast<char>('0' + tt.tm_mday % 10);
        date[10] = ' ';
        date[11] = static_cast<char>('0' + tt.tm_hour / 10);
        date[12] = static_cast<char>('0' + tt.tm_hour % 10);
        date[13] = ':';
        date[14] = static_cast<char>('0' + tt.tm_min / 10);
        date[15] = static_cast<char>('0' + tt.tm_min % 10);
        date[16] = ':';
        date[17] = static_cast<char>('0' + tt.tm_sec / 10);
        date[18] = static_cast<char>('0' + tt.tm_sec % 10);
        date[19] = '.';

        psec = sec;
      }

      entry.append(date, 20);

      entry += static_cast<char>('0' + t.tv_usec / 100000 % 10);
      entry += static_cast<char>('0' + t.tv_usec / 10000 % 10);
      entry += static_cast<char>('0' + t.tv_usec / 1000 % 10);
      entry += static_cast<char>('0' + t.tv_usec / 100 % 10);
      entry += static_cast<char>('0' + t.tv_usec / 10 % 10);
      entry += ' ';
      entry += '[';
      char str[64];
      char* p = putInt(str, getpid());
      entry.append(str, p - str);
      entry += '.';
      p = putInt(str, pthread_self());
      entry.append(str, p - str);
      entry += "] ";
      entry += level;
      entry += ' ';
      entry += category;
      entry += " - ";
    }

    class LogAppender : public RefCounted
    {
      public:
        virtual ~LogAppender() { }
        virtual void putMessage(const std::string& msg) = 0;
        virtual void finish(bool flush) = 0;
    };

    //////////////////////////////////////////////////////////////////////
    // OStreamAppender
    //
    class OStreamAppender : public LogAppender
    {
        std::ostream _out;

      public:
        explicit OStreamAppender(std::ostream& out)
          : _out(out.rdbuf())
        { }

        virtual void putMessage(const std::string& msg);
        virtual void finish(bool flush);
    };

    void OStreamAppender::putMessage(const std::string& msg)
    {
      _out << msg << '\n';
    }

    void OStreamAppender::finish(bool flush)
    {
      if (flush)
        _out.flush();
    }

    //////////////////////////////////////////////////////////////////////
    // FileAppender
    //
    class FileAppender : public LogAppender
    {
      protected:
        std::string _fname;
        std::ofstream _ofile;

      public:
        explicit FileAppender(const std::string& fname);
        virtual void putMessage(const std::string& msg);
        virtual void finish(bool flush);
    };

    FileAppender::FileAppender(const std::string& fname)
      : _fname(fname),
        _ofile(fname.c_str(), std::ios::out | std::ios::app)
    {
    }

    void FileAppender::putMessage(const std::string& msg)
    {
      if (!_ofile.is_open())
      {
        _ofile.clear();
        _ofile.open(_fname.c_str(), std::ios::out | std::ios::app);
      }

      _ofile << msg << '\n';
    }

    void FileAppender::finish(bool flush)
    {
      if (flush)
        _ofile.flush();
    }

    //////////////////////////////////////////////////////////////////////
    // RollingFileAppender
    //
    class RollingFileAppender : public FileAppender
    {
        unsigned _maxfilesize;
        unsigned _maxbackupindex;

        unsigned _fsize;

        void doRotate();
        std::string mkfilename(unsigned idx) const;

      public:
        RollingFileAppender(const std::string& fname, unsigned maxfilesize, unsigned maxbackupindex);
        virtual void putMessage(const std::string& msg);
    };

    RollingFileAppender::RollingFileAppender(const std::string& fname, unsigned maxfilesize, unsigned maxbackupindex)
      : FileAppender(fname),
        _maxfilesize(maxfilesize),
        _maxbackupindex(maxbackupindex),
        _fsize(_ofile.tellp())
    {
    }

    void RollingFileAppender::doRotate()
    {
      _ofile.clear();
      _ofile.close();

      // ignore unlink- and rename-errors. In case of failure the
      // original file is reopened

      std::string newfilename = mkfilename(_maxbackupindex);
      ::unlink(newfilename.c_str());
      for (unsigned idx = _maxbackupindex; idx > 0; --idx)
      {
        std::string oldfilename = mkfilename(idx - 1);
        ::rename(oldfilename.c_str(), newfilename.c_str());
        newfilename = oldfilename;
      }

      ::rename(_fname.c_str(), newfilename.c_str());

      _ofile.open(_fname.c_str(), std::ios::out | std::ios::app);
      _fsize = 0;
    }

    std::string RollingFileAppender::mkfilename(unsigned idx) const
    {
      std::string fname(_fname);
      fname += '.';
      fname += convert<std::string>(idx);
      return fname;
    }

    void RollingFileAppender::putMessage(const std::string& msg)
    {
      if (_fsize >= _maxfilesize)
        doRotate();
      FileAppender::putMessage(msg);
      _fsize += msg.size() + 1;  // FileAppender adds line feed to the message
    }

    //////////////////////////////////////////////////////////////////////
    // UdpAppender
    //
    class UdpAppender : public LogAppender
    {
        net::UdpSender _loghost;
        std::string _msg;

      public:
        UdpAppender(const std::string& host, unsigned short int port, bool broadcast = true)
          : _loghost(host, port, broadcast)
        { }

        virtual void putMessage(const std::string& msg);
        virtual void finish(bool flush);
    };

    void UdpAppender::putMessage(const std::string& msg)
    {
      _msg = msg;
    }

    void UdpAppender::finish(bool flush)
    {
      try
      {
        _loghost.send(_msg);
      }
      catch (const std::exception&)
      {
      }
      _msg.clear();
    }

    //////////////////////////////////////////////////////////////////////
    Logger::log_level_type str2loglevel(const std::string& level, const std::string& category = std::string())
    {
      char l = level.empty() ? '\0' : level[0];
      switch (l)
      {
        case 'f':
        case 'F': return Logger::LOG_LEVEL_FATAL;
        case 'e':
        case 'E': return Logger::LOG_LEVEL_ERROR;
        case 'w':
        case 'W': return Logger::LOG_LEVEL_WARN;
        case 'i':
        case 'I': return Logger::LOG_LEVEL_INFO;
        case 'd':
        case 'D': return Logger::LOG_LEVEL_DEBUG;
        case 't':
        case 'T': return Logger::LOG_LEVEL_TRACE;
        default:
                  {
                    std::string msg = "unknown log level \"" + level + '\"';
                    if (!category.empty())
                      msg += " for category \"" + category + '"';
                    throw std::runtime_error(msg);
                  }
      }
    }
  }

  //////////////////////////////////////////////////////////////////////
  // Logger
  //

  //////////////////////////////////////////////////////////////////////
  // LoggerManagerConfiguration
  //

  class LoggerManagerConfiguration::Impl
  {
    public:
      typedef std::map<std::string, Logger::log_level_type> LogLevels;

    private:
      friend void operator>>= (const SerializationInfo& si, LoggerManagerConfiguration::Impl& loggerManagerConfigurationImpl);
      std::string _fname;
      unsigned _maxfilesize;
      unsigned _maxbackupindex;
      std::string _loghost;
      unsigned short _logport;
      bool _broadcast;

      Logger::log_level_type _rootLevel;
      LogLevels _logLevels;

    public:
      Impl()
        : _maxfilesize(0),
          _maxbackupindex(0),
          _logport(0),
          _broadcast(true),
          _rootLevel(Logger::LOG_LEVEL_FATAL)
      { }

      const std::string& fname() const          { return _fname; }
      unsigned maxfilesize() const              { return _maxfilesize; }
      unsigned maxbackupindex() const           { return _maxbackupindex; }
      const std::string& loghost() const        { return _loghost; }
      unsigned short logport() const            { return _logport; }
      bool broadcast() const                    { return _broadcast; }

      Logger::log_level_type rootLevel() const  { return _rootLevel; }
      Logger::log_level_type logLevel(const std::string& category) const;
      const LogLevels& logLevels() const        { return _logLevels; }
  };

  LoggerManagerConfiguration::LoggerManagerConfiguration()
    : _impl(new LoggerManagerConfiguration::Impl())
  {
  }

  LoggerManagerConfiguration::LoggerManagerConfiguration(const LoggerManagerConfiguration& c)
    : _impl(new Impl(*c._impl))
  {
  }

  LoggerManagerConfiguration& LoggerManagerConfiguration::operator=(const LoggerManagerConfiguration& c)
  {
    delete _impl;
    _impl = 0;
    _impl = new Impl(*c._impl);
    return *this;
  }

  LoggerManagerConfiguration::~LoggerManagerConfiguration()
  {
    delete _impl;
  }

  Logger::log_level_type LoggerManagerConfiguration::rootLevel() const
  {
    return _impl->rootLevel();
  }

  Logger::log_level_type LoggerManagerConfiguration::logLevel(const std::string& category) const
  {
    return _impl->logLevel(category);
  }

  Logger::log_level_type LoggerManagerConfiguration::Impl::logLevel(const std::string& category) const
  {
    // check for exact match of category in log level settings
    LogLevels::const_iterator lit = _logLevels.find(category);
    if (lit != _logLevels.end())
      return lit->second;

    // find best match of category in log level settings
    std::string::size_type best_len = 0;
    Logger::log_level_type best_level = _rootLevel;

    for (LogLevels::const_iterator it = _logLevels.begin(); it != _logLevels.end(); ++it)
    {
      if (it->first.size() > best_len
        && it->first.size() < category.size()
        && category.at(it->first.size()) == '.'
        && category.compare(0, it->first.size(), it->first) == 0)
      {
        best_len = it->first.size();
        best_level = it->second;
      }
    }

    return best_level;
  }

  void operator>>= (const SerializationInfo& si, LoggerManagerConfiguration::Impl& impl)
  {
    if (si.getMember("file", impl._fname))
    {
      std::string s;
      if (si.getMember("maxfilesize", s))
      {
        bool ok = true;
        std::string::iterator it = getInt(s.begin(), s.end(), ok, impl._maxfilesize);
        if (!ok)
          throw std::runtime_error("failed to read maxfilesize (\"" + s + "\")");
        if (it != s.end())
        {
          switch (*it)
          {
            case 'k':
            case 'K':
              impl._maxfilesize *= 1024;
              break;

            case 'm':
            case 'M':
              impl._maxfilesize *= 1024 * 1024;
              break;

            case 'g':
            case 'G':
              impl._maxfilesize *= 1024 * 1024 * 1024;
              break;
          }
        }

        si.getMember("maxbackupindex") >>= impl._maxbackupindex;
      }
    }
    else if (si.getMember("logport", impl._logport))
    {
      si.getMember("loghost", impl._loghost);
      si.getMember("broadcast", impl._broadcast);
    }

    std::string rootLevel;
    if (!si.getMember("rootlogger", rootLevel))
      impl._rootLevel = Logger::LOG_LEVEL_FATAL;
    else
      impl._rootLevel = str2loglevel(rootLevel);

    const SerializationInfo* psi = si.findMember("loggers");
    if (psi)
    {
      std::string category;
      std::string levelstr;
      Logger::log_level_type level;
      for( SerializationInfo::ConstIterator it = psi->begin(); it != psi->end(); ++it)
      {
        it->getMember("category") >>= category;
        if (impl._logLevels.find(category) != impl._logLevels.end())
          throw std::runtime_error("level already set for category \"" + category + '"'); 

        it->getMember("level") >>= levelstr;
        if (levelstr.empty())
          level = Logger::LOG_LEVEL_FATAL;
        else
          level = str2loglevel(levelstr, category);

        impl._logLevels[category] = level;
      }
    }
  }

  void operator>>= (const SerializationInfo& si, LoggerManagerConfiguration& loggerManagerConfiguration)
  {
    si >>= *loggerManagerConfiguration.impl();
  }

  //////////////////////////////////////////////////////////////////////
  // LoggerManager
  //
  class LoggerManager::Impl
  {
      SmartPtr<LogAppender> _appender;
      LoggerManagerConfiguration _config;
      typedef std::map<std::string, Logger*> Loggers;  // map category => logger
      Loggers _loggers;

      Impl(const Impl&);
      Impl& operator=(const Impl&);

    public:
      explicit Impl(const LoggerManagerConfiguration& config);

      Logger* getLogger(const std::string& category);
      LogAppender& appender()
      { return *_appender; }
    
      Logger::log_level_type rootLevel() const
      { return _config.rootLevel(); }
      Logger::log_level_type logLevel(const std::string& category) const
      { return _config.logLevel(category); }
  };

  LoggerManager::Impl::Impl(const LoggerManagerConfiguration& config)
  {
    if (config.impl()->fname().empty())
    {
      if (config.impl()->logport() != 0)
      {
        _appender = new UdpAppender(config.impl()->loghost(), config.impl()->logport(), config.impl()->broadcast());
      }
      else
      {
        _appender = new OStreamAppender(std::cerr);
      }
    }
    else if (config.impl()->maxfilesize() == 0)
    {
      _appender = new FileAppender(config.impl()->fname());
    }
    else
    {
      _appender = new RollingFileAppender(config.impl()->fname(), config.impl()->maxfilesize(), config.impl()->maxbackupindex());
    }

    _config = config;
  }

  bool LoggerManager::_enabled = false;

  LoggerManager::LoggerManager()
  {
  }

  LoggerManager::~LoggerManager()
  {
    _enabled = false;
  }

  LoggerManager& LoggerManager::getInstance()
  {
    static LoggerManager loggerManager;
    return loggerManager;
  }

  void LoggerManager::logInit(const std::string& fname)
  {
    std::ifstream in(fname.c_str());
    if (in)
    {
      xml::XmlDeserializer d(in);
      LoggerManagerConfiguration config;
      d.deserialize(config);
      getInstance().configure(config);
    }
  }

  void LoggerManager::logInit(const cxxtools::SerializationInfo& si)
  {
    LoggerManagerConfiguration config;
    si >>= config;
    getInstance().configure(config);
  }

  void LoggerManager::configure(const LoggerManagerConfiguration& config)
  {
    Impl* p = new Impl(config);
    delete _impl;
    _impl = p;
    _enabled = true;
  }

  Logger::log_level_type LoggerManager::rootLevel() const
  {
    return _impl->rootLevel();
  }

  Logger::log_level_type LoggerManager::logLevel(const std::string& category) const
  {
    return _impl->logLevel(category);
  }

  Logger* LoggerManager::getLogger(const std::string& category)
  {
    if (_impl == 0)
      return 0;
    return _impl->getLogger(category);
  }

  Logger* LoggerManager::Impl::getLogger(const std::string& category)
  {
    MutexLock lock(loggersMutex);

    // check for existing loggers
    Loggers::iterator it = _loggers.find(category);
    if (it != _loggers.end())
      return it->second;

    Logger* ret = new Logger(category, logLevel(category));
    _loggers[category] = ret;

    return ret;
  }

  //////////////////////////////////////////////////////////////////////
  // LogMessage
  //
  class LogMessage::Impl
  {
      Logger* _logger;
      const char* _level;
      std::ostringstream _msg;

    public:
      void setLogger(Logger* logger)
      { _logger = logger; }

      void setLevel(const char* level)
      { _level = level; }

      void finish();

      std::ostringstream& out()
      { return _msg; }

      std::string str()
      { return _msg.str(); }

      void clear()
      {
        _msg.clear();
        _msg.str(std::string());
      }
  };

  namespace
  {
    LPool<LogMessage::Impl> logMessageImplPool;
  }

  LogMessage::LogMessage(Logger* logger, const char* level)
    : _impl(logMessageImplPool.getInstance())
  {
    _impl->setLogger(logger);
    _impl->setLevel(level);
  }

  LogMessage::LogMessage(Logger* logger, Logger::log_level_type level)
    : _impl(logMessageImplPool.getInstance())
  {
    _impl->setLogger(logger);
    _impl->setLevel(level >= Logger::LOG_LEVEL_TRACE ? "TRACE"
                  : level >= Logger::LOG_LEVEL_DEBUG ? "DEBUG"
                  : level >= Logger::LOG_LEVEL_INFO  ? "INFO"
                  : level >= Logger::LOG_LEVEL_WARN  ? "WARN"
                  : level >= Logger::LOG_LEVEL_ERROR ? "ERROR"
                  : "FATAL");
  }

  LogMessage::~LogMessage()
  {
    if (_impl)
    {
      _impl->finish();
      logMessageImplPool.releaseInstance(_impl);
    }
  }

  void LogMessage::finish()
  {
    _impl->finish();
    logMessageImplPool.releaseInstance(_impl);
    _impl = 0;
  }

  void LogMessage::Impl::finish()
  {
    try
    {
      ScopedAtomicIncrementer inc(mutexWaitCount);
      MutexLock lock(logMutex);

      std::string msg;
      logentry(msg, _level, _logger->getCategory());
      msg += _msg.str();

      LogAppender& appender = LoggerManager::getInstance().impl()->appender();
      appender.putMessage(msg);
      appender.finish((atomicGet(mutexWaitCount) <= 1));
    }
    catch (const std::exception&)
    {
    }

    clear();
  }

  std::ostream& LogMessage::out()
  {
    return _impl->out();
  }

  std::string LogMessage::str() const
  {
    return _impl->str();
  }

  //////////////////////////////////////////////////////////////////////
  // LogTracer
  //
  class LogTracer::Impl
  {
      std::ostringstream _msg;
      Logger* _logger;

      void putmessage(const char* state) const;

    public:
      explicit Impl(Logger* logger)
        : _logger(logger)
      { }

      void setLogger(Logger* logger)
      { _logger = logger; }

      std::ostream& out()
      { return _msg; }

      void enter() const
      { putmessage("ENTER "); }

      void exit() const
      { putmessage("EXIT "); }
  };

  LogTracer::LogTracer()
    : _impl(0)
  { }

  LogTracer::~LogTracer()
  {
    if (_impl)
    {
      _impl->exit();
      delete _impl;
    }
  }

  void LogTracer::setLogger(Logger* l)
  {
    if (_impl)
      _impl->setLogger(l);
    else
      _impl = new Impl(l);
  }

  std::ostream& LogTracer::out()
  {
    return _impl->out();
  }

  void LogTracer::enter()
  {
    if (_impl)
      _impl->enter();
  }

  void LogTracer::exit()
  {
    if (_impl)
    {
      _impl->exit();
      delete _impl;
      _impl = 0;
    }
  }

  void LogTracer::Impl::putmessage(const char* state) const
  {
    try
    {
      ScopedAtomicIncrementer inc(mutexWaitCount);
      MutexLock lock(logMutex);

      std::string msg;
      logentry(msg, "TRACE", _logger->getCategory());
      msg += state;
      msg += _msg.str();

      LogAppender& appender = LoggerManager::getInstance().impl()->appender();
      appender.putMessage(msg);
      appender.finish((atomicGet(mutexWaitCount) <= 1));
    }
    catch (const std::exception&)
    {
    }
  }

}
