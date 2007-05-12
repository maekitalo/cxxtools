/* log.cpp
 *
 * cxxtools - general purpose C++-toolbox
 * Copyright (C) 2003,2004 Tommi Maekitalo
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <cxxtools/log/cxxtools_init.h>
#include <cxxtools/thread.h>
#include <cxxtools/udp.h>
#include <cxxtools/udpstream.h>
#include <cxxtools/tee.h>
#include <cxxtools/streamcounter.h>
#include <list>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <locale>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <cctype>

namespace cxxtools
{
  class LoggerImpl : public Logger
  {
      static std::string fname;
      static std::ofstream outfile;
      static cxxtools::Streamcounter counter;
      static cxxtools::Tee tee;
      static net::UdpSender loghost;
      static net::UdpOStream udpmessage;
      static unsigned maxfilesize;
      static unsigned maxbackupindex;

      static std::string mkfilename(unsigned idx);

      class FlusherThread : public DetachedThread
      {
          std::ostream& out;
          static struct timespec flushDelay;

        protected:
          void run();

        public:
          FlusherThread(std::ostream& out_)
            : out(out_)
            { }

          static void setFlushDelay(unsigned ms)
          {
            flushDelay.tv_sec = ms / 1000;
            flushDelay.tv_nsec = ms % 1000;
          }
      };

      static FlusherThread* flusherThread;
      static bool flusherThreadStarted;

    public:
      LoggerImpl(const std::string& c, log_level_type l)
        : Logger(c, l)
        { }
      static std::ostream& getAppender();
      void logEnd(std::ostream& appender);
      static void doRotate();
      static void setFile(const std::string& fname);
      static void setFlushDelay(unsigned ms);
      static void setMaxFileSize(unsigned size)   { maxfilesize = size; }
      static void setMaxBackupIndex(unsigned idx) { maxbackupindex = idx; }
      static void setLoghost(const std::string& host, unsigned short int port);
  };

  std::ostream& LoggerImpl::getAppender()
  {
    if (!fname.empty())
    {
      if (!outfile.is_open())
      {
        outfile.clear();
        outfile.open(fname.c_str(), std::ios::out | std::ios::app);
      }

      if (maxfilesize > 0)
      {
        if (counter.getCount() > maxfilesize)
        {
          doRotate();
          counter.resetCount();
        }
        return tee;
      }
      else
        return outfile;
    }
    else if (loghost.isConnected())
      return udpmessage;
    else
      return std::cout;
  }

  void LoggerImpl::logEnd(std::ostream& appender)
  {
    if (flusherThread == 0)
      appender.flush();
    else if (!flusherThreadStarted && !fname.empty())
    {
      flusherThread->create();
      flusherThreadStarted = true;
    }
  }

  std::string LoggerImpl::mkfilename(unsigned idx)
  {
    std::ostringstream f;
    f << fname << '.' << idx;
    return f.str();
  }

  void LoggerImpl::doRotate()
  {
    outfile.clear();
    outfile.close();

    // ignore unlink- and rename-errors. In case of failure the
    // original file is reopened

    std::string newfilename = mkfilename(maxbackupindex);
    ::unlink(newfilename.c_str());
    for (unsigned idx = maxbackupindex; idx > 0; --idx)
    {
      std::string oldfilename = mkfilename(idx - 1);
      ::rename(oldfilename.c_str(), newfilename.c_str());
      newfilename = oldfilename;
    }

    ::rename(fname.c_str(), newfilename.c_str());

    outfile.open(fname.c_str(), std::ios::out | std::ios::app);
    counter.resetCount(outfile.tellp());
  }

  std::string LoggerImpl::fname;
  std::ofstream LoggerImpl::outfile;
  cxxtools::Streamcounter LoggerImpl::counter;
  cxxtools::Tee LoggerImpl::tee(LoggerImpl::outfile, LoggerImpl::counter);
  net::UdpSender LoggerImpl::loghost;
  net::UdpOStream LoggerImpl::udpmessage(LoggerImpl::loghost);
  unsigned LoggerImpl::maxfilesize = 0;
  unsigned LoggerImpl::maxbackupindex = 0;
  LoggerImpl::FlusherThread* LoggerImpl::flusherThread = 0;
  bool LoggerImpl::flusherThreadStarted = false;

  struct timespec LoggerImpl::FlusherThread::flushDelay;

  void LoggerImpl::setFile(const std::string& fname_)
  {
    fname = fname_;

    struct stat s;
    int ret = stat(fname_.c_str(), &s);
    counter.resetCount(ret == 0 ? s.st_size : 0);
  }

  void LoggerImpl::setFlushDelay(unsigned ms)
  {
    FlusherThread::setFlushDelay(ms);
    if (ms > 0 && flusherThread == 0 && !fname.empty())
      flusherThread = new FlusherThread(outfile);
    else if (ms == 0)
      flusherThread = 0;
  }

  void LoggerImpl::setLoghost(const std::string& host, unsigned short int port)
  {
    loghost.connect(host.c_str(), port, true);
  }

  RWLock Logger::rwmutex;
  Mutex Logger::mutex;
  Logger::log_level_type Logger::std_level = LOG_LEVEL_ERROR;
  bool Logger::enabled = false;

  namespace
  {
    typedef std::list<Logger*> loggers_type;

    loggers_type& getBaseLoggers()
    {
      static loggers_type* baseLoggers = 0;
      if (baseLoggers == 0)
      {
        MutexLock lock(Logger::mutex);
        if (baseLoggers == 0)
          baseLoggers = new loggers_type();
      }
      return *baseLoggers;
    }

    loggers_type& getCacheLoggers()
    {
      static loggers_type* cacheLoggers = 0;
      if (cacheLoggers == 0)
      {
        MutexLock lock(Logger::mutex);
        if (cacheLoggers == 0)
          cacheLoggers = new loggers_type();
      }
      return *cacheLoggers;
    }

    Logger::log_level_type getBaseLogLevel(const std::string& category)
    {
      // search best-fit Logger
      std::string::size_type best_len = 0;
      Logger::log_level_type best_level = Logger::getStdLevel();

      loggers_type& baseLoggers = getBaseLoggers();
      for (loggers_type::iterator it = baseLoggers.begin();
           it != baseLoggers.end(); ++it)
      {
        if ((*it)->getCategory() == category)
        {
          best_level = (*it)->getLogLevel();
          break;
        }
        else if ((*it)->getCategory().size() > best_len
          && (*it)->getCategory().size() < category.size()
          && category.at((*it)->getCategory().size()) == '.'
          && category.compare(0, (*it)->getCategory().size(), (*it)->getCategory()) == 0)
        {
          best_len = (*it)->getCategory().size();
          // update log-level
          best_level = (*it)->getLogLevel();
        }
      }

      return best_level;
    }

    void reinitializeLoggers()
    {
      // reinitialize already instantiated loggers
      for (loggers_type::iterator it = getCacheLoggers().begin();
           it != getCacheLoggers().end(); ++it)
        (*it)->setLogLevel(getBaseLogLevel((*it)->getCategory()));
    }
  }

  Logger* Logger::getLogger(const std::string& category)
  {
    if (!enabled)
      return 0;

    // search existing Logger
    RdLock rdLock(rwmutex);

    loggers_type::iterator lower_bound_it = getCacheLoggers().begin();
    while (lower_bound_it != getCacheLoggers().end()
        && (*lower_bound_it)->getCategory() < category)
      ++lower_bound_it;

    if (lower_bound_it != getCacheLoggers().end()
     && (*lower_bound_it)->getCategory() == category)
        return *lower_bound_it;

    // Logger not in list - change to write-lock
    rdLock.unlock();
    WrLock wrLock(rwmutex);

    // we have to do it again after gaining write-lock
    lower_bound_it = getCacheLoggers().begin();
    while (lower_bound_it != getCacheLoggers().end()
        && (*lower_bound_it)->getCategory() < category)
      ++lower_bound_it;

    if (lower_bound_it != getCacheLoggers().end()
     && (*lower_bound_it)->getCategory() == category)
        return *lower_bound_it;

    // Logger still not in list, but we have a position to insert

    log_level_type base_level = getBaseLogLevel(category);

    // insert the new Logger in list and return pointer to the new list-element
    return *(getCacheLoggers().insert(lower_bound_it, new LoggerImpl(category, base_level)));
  }

  Logger* Logger::setLevel(const std::string& category, log_level_type l)
  {
    WrLock lock(rwmutex);

    // search for existing Logger
    loggers_type& baseLoggers = getBaseLoggers();
    loggers_type::iterator it = baseLoggers.begin();
    while (it != baseLoggers.end()
        && (*it)->getCategory() < category)
      ++it;

    if (it == baseLoggers.end() || (*it)->getCategory() != category)
    {
      // Logger not found - create new
      it = baseLoggers.insert(it, new LoggerImpl(category, l));
    }
    else
      (*it)->setLogLevel(l); // Logger found - set level

    // return pointer to object in list
    return *it;
  }

  namespace
  {
    static char digits[] = "0123456789";

    inline char hiDigit(int i)
    { return digits[i / 10]; }

    inline char loDigit(int i)
    { return digits[i % 10]; }
  }

  std::ostream& Logger::logentry(const char* level)
  {
    struct timeval t;
    gettimeofday(&t, 0);

    // format date only once per second:
    static char date[21];
    static time_t psec = 0;
    time_t sec = static_cast<time_t>(t.tv_sec);
    if (sec != psec)
    {
      struct tm tt;
      localtime_r(&sec, &tt);
      psec = sec;
      date[0] = digits[(1900 + tt.tm_year) / 1000];
      date[1] = loDigit((1900 + tt.tm_year) / 100);
      date[2] = loDigit((1900 + tt.tm_year) / 10);
      date[3] = loDigit(1900 + tt.tm_year);
      date[4] = '-';
      date[5] = hiDigit(tt.tm_mon + 1);
      date[6] = loDigit(tt.tm_mon + 1);
      date[7] = '-';
      date[8] = hiDigit(tt.tm_mday);
      date[9] = loDigit(tt.tm_mday);
      date[10] = ' ';
      date[11] = hiDigit(tt.tm_hour);
      date[12] = loDigit(tt.tm_hour);
      date[13] = ':';
      date[14] = hiDigit(tt.tm_min);
      date[15] = loDigit(tt.tm_min);
      date[16] = ':';
      date[17] = hiDigit(tt.tm_sec);
      date[18] = loDigit(tt.tm_sec);
      date[19] = '.';
      date[20] = '\0';
    }

    std::ostream& out = LoggerImpl::getAppender();

    out.clear();

    out << date
        << loDigit(t.tv_usec / 100000) << loDigit(t.tv_usec / 10000)
        << loDigit(t.tv_usec / 1000) << loDigit(t.tv_usec / 100)
        << loDigit(t.tv_usec / 10)
        << " [" << getpid() << '.' << pthread_self() << "] "
        << level << ' '
        << category << " - ";

    return out;
  }

  class LogMessage::LogMessageImpl
  {
      std::ostringstream msg;
      Logger* logger;
      const char* level;

    public:
      LogMessageImpl(Logger* logger_, const char* level_)
        : logger(logger_),
          level(level_)
          { }

      std::ostream& out()     { return msg; }
      void flush()
      {
        MutexLock lock(Logger::mutex);

        std::ostream& out(logger->logentry(level));
        out << msg.str() << '\n';

        logger->logEnd(out);
      }
  };

  LogMessage::LogMessage(Logger* logger, const char* level)
    : impl(new LogMessageImpl(logger, level))
    { }

  void LogMessage::flush()
  {
    impl->flush();
  }

  LogMessage::~LogMessage()
  {
    delete impl;
  }

  std::ostream& LogMessage::out()
  { return impl->out(); }

  LogTracer::~LogTracer()
  {
    if (msg)
    {
      if (l->isEnabled(Logger::LOG_LEVEL_TRACE))
      {
        cxxtools::MutexLock lock(cxxtools::Logger::mutex);
        l->logentry("TRACE")
          << "EXIT " << msg->str() << std::endl;
      }
      delete msg;
    }
  }

  std::ostream& LogTracer::logentry()
  {
    if (!msg)
      msg = new std::ostringstream();
    return *msg;
  }

  void LogTracer::enter()
  {
    if (msg && l->isEnabled(Logger::LOG_LEVEL_TRACE))
    {
      cxxtools::MutexLock lock(cxxtools::Logger::mutex);
      l->logentry("TRACE")
        << "ENTER " << msg->str() << std::endl;
    }
  }

  void LoggerImpl::FlusherThread::run()
  {
    while (flushDelay.tv_sec > 0 || flushDelay.tv_nsec > 0)
    {
      nanosleep(&flushDelay, 0);
      cxxtools::MutexLock lock(Logger::mutex);
      out.flush();
    }
  }

}

void log_init_cxxtools(cxxtools::Logger::log_level_type level)
{
  cxxtools::Logger::setEnabled(false);

  cxxtools::Logger::setRootLevel(level);

  cxxtools::Logger::setEnabled(true);
  cxxtools::reinitializeLoggers();
}

void log_init_cxxtools(const std::string& propertyfilename)
{
  cxxtools::Logger::setEnabled(false);
  cxxtools::getBaseLoggers().clear();

  std::ifstream in(propertyfilename.c_str());
  log_init_cxxtools(in);
}

void log_init_cxxtools(std::istream& in)
{
  cxxtools::Logger::setEnabled(true);

  enum state_type {
    state_0,
    state_token,
    state_tokensp,
    state_category,
    state_level,
    state_rootlevel,
    state_filename0,
    state_filename,
    state_host0,
    state_host,
    state_port,
    state_fsize0,
    state_fsize,
    state_maxbackupindex0,
    state_maxbackupindex,
    state_flushdelay0,
    state_flushdelay,
    state_disable,
    state_skip
  };
  
  state_type state = state_0;

  char ch;
  std::string token;
  std::string category;
  std::string filename;
  std::string host;
  unsigned short int port;
  unsigned fsize;
  unsigned maxbackupindex;
  unsigned flushdelay = 0;

  cxxtools::Logger::log_level_type level;
  while (in.get(ch))
  {
    switch (state)
    {
      case state_0:
        if (std::isalnum(ch) || ch == '_')
        {
          token = std::toupper(ch);
          state = state_token;
        }
        else if (!std::isspace(ch))
          state = state_skip;
        break;

      case state_token:
        if (ch == '.')
        {
          if (token == "LOGGER")
            state = state_category;
          else
          {
            token.clear();
            state = state_token;
          }
        }
        else if (ch == '=' && token == "ROOTLOGGER")
          state = state_rootlevel;
        else if (ch == '=' && token == "FILE")
          state = state_filename0;
        else if (ch == '=' && token == "HOST")
          state = state_host0;
        else if (ch == '=' && token == "MAXFILESIZE")
          state = state_fsize0;
        else if (ch == '=' && token == "MAXBACKUPINDEX")
          state = state_maxbackupindex0;
        else if (ch == '=' && token == "FLUSHDELAY")
          state = state_flushdelay0;
        else if (ch == '=' && token == "DISABLED")
          state = state_disable;
        else if (ch == '\n')
          state = state_0;
        else if (std::isspace(ch))
          state = state_tokensp;
        else if (std::isalnum(ch) || ch == '_')
          token += std::toupper(ch);
        else
        {
          token.clear();
          state = state_skip;
        }
        break;

      case state_tokensp:
        if (ch == '=' && token == "ROOTLOGGER")
          state = state_rootlevel;
        else if (ch == '=' && token == "FILE")
          state = state_filename0;
        else if (ch == '=' && token == "HOST")
          state = state_host0;
        else if (ch == '=' && token == "MAXFILESIZE")
          state = state_fsize0;
        else if (ch == '=' && token == "MAXBACKUPINDEX")
          state = state_maxbackupindex0;
        else if (ch == '=' && token == "FLUSHDELAY")
          state = state_flushdelay0;
        else if (ch == '=' && token == "DISABLE")
          state = state_disable;
        else if (ch == '\n')
          state = state_0;
        else if (!std::isspace(ch))
          state = state_skip;
        break;

      case state_category:
        if (std::isalnum(ch) || ch == '_' || ch == '.')
          category += ch;
        else if (ch == '=')
          state = state_level;
        else
        {
          category.clear();
          token.clear();
          state = (ch == '\n' ? state_0 : state_skip);
        }
        break;

      case state_level:
      case state_rootlevel:
        if (ch != '\n' && std::isspace(ch))
          break;

        switch (ch)
        {
          case 'F': level = cxxtools::Logger::LOG_LEVEL_FATAL; break;
          case 'E': level = cxxtools::Logger::LOG_LEVEL_ERROR; break;
          case 'W': level = cxxtools::Logger::LOG_LEVEL_WARN; break;
          case 'I': level = cxxtools::Logger::LOG_LEVEL_INFO; break;
          case 'D': level = cxxtools::Logger::LOG_LEVEL_DEBUG; break;
          case 'T': level = cxxtools::Logger::LOG_LEVEL_TRACE; break;
          default:  level = cxxtools::Logger::getStdLevel(); break;
        }
        if (state == state_rootlevel)
          cxxtools::Logger::setRootLevel(level);
        else
          cxxtools::Logger::setLevel(category, level);
        category.clear();
        token.clear();
        state = state_skip;
        break;

      case state_filename0:
        if (ch != '\n' && std::isspace(ch))
          break;

        state = state_filename;

      case state_filename:
        if (ch == '\n')
        {
          cxxtools::LoggerImpl::setFile(filename);
          token.clear();
          filename.clear();
          state = state_0;
        }
        else
          filename += ch;
        break;

      case state_host0:
        if (ch == '\n')
        {
          state = state_0;
          break;
        }
        else if (std::isspace(ch))
          break;

        state = state_host;

      case state_host:
        if (ch == ':')
        {
          port = 0;
          state = state_port;
        }
        else if (std::isspace(ch))
          state = state_skip;
        else
          host += ch;
        break;

      case state_port:
        if (std::isdigit(ch))
          port = port * 10 + ch - '0';
        else if (port > 0)
        {
          cxxtools::LoggerImpl::setLoghost(host, port);
          state = (ch == '\n' ? state_0 : state_skip);
        }
        break;

      case state_fsize0:
        if (ch == '\n')
        {
          state = state_0;
          break;
        }
        else if (std::isspace(ch))
          break;

        state = state_fsize;
        fsize = 0;

      case state_fsize:
        if (std::isdigit(ch))
          fsize = fsize * 10 + ch - '0';
        else if (ch == '\n')
        {
          cxxtools::LoggerImpl::setMaxFileSize(fsize);
          state = state_0;
        }
        else
        {
          if (ch == 'k' || ch == 'K')
            fsize *= 1024;
          else if (ch == 'M')
            fsize *= 1024 * 1024;

          cxxtools::LoggerImpl::setMaxFileSize(fsize);
          state = state_skip;
        }
        break;

      case state_maxbackupindex0:
        if (ch == '\n')
        {
          state = state_0;
          break;
        }
        else if (std::isspace(ch))
          break;

        state = state_maxbackupindex;
        maxbackupindex = 0;

      case state_maxbackupindex:
        if (std::isdigit(ch))
          maxbackupindex = maxbackupindex * 10 + ch - '0';
        else
        {
          cxxtools::LoggerImpl::setMaxBackupIndex(maxbackupindex);
          state = (ch == '\n' ? state_0 : state_skip);
        }
        break;

      case state_flushdelay0:
        if (ch == '\n')
        {
          state = state_0;
          break;
        }
        else if (std::isdigit(ch))
        {
          flushdelay = ch - '0';
          state = state_flushdelay;
        }
        else if (!std::isspace(ch))
          state = state_skip;
        break;

      case state_flushdelay:
        if (std::isdigit(ch))
          flushdelay = flushdelay * 10 + ch - '0';
        else
          state = (ch == '\n' ? state_0 : state_skip);
        break;

      case state_disable:
        if (ch == '1' || ch == 't' || ch == 'T' || ch == 'y' || ch == 'Y')
        {
          cxxtools::Logger::setEnabled(false);
          state = state_skip;
        }
        else if (ch != ' ' && ch != '\t')
          state = (ch == '\n' ? state_0 : state_skip);
        break;

      case state_skip:
        if (ch == '\n')
          state = state_0;
        break;
    }
  }

  cxxtools::LoggerImpl::setFlushDelay(flushdelay);
  cxxtools::reinitializeLoggers();
}

void log_init_cxxtools()
{
  char* LOGPROPERTIES = ::getenv("LOGPROPERTIES");
  if (LOGPROPERTIES)
    log_init_cxxtools(LOGPROPERTIES);
  else
  {
    struct stat s;
    if (stat("log.properties", &s) == 0)
      log_init_cxxtools("log.properties");
    else
      log_init_cxxtools(cxxtools::Logger::LOG_LEVEL_ERROR);
  }
}

