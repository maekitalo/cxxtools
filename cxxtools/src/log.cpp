/*
 * Copyright (C) 2003,2004 Tommi Maekitalo
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

#include <cxxtools/log/cxxtools_init.h>
#include <cxxtools/mutex.h>
#include <cxxtools/net/udp.h>
#include <cxxtools/net/udpstream.h>
#include <cxxtools/tee.h>
#include <cxxtools/streamcounter.h>
#include <cxxtools/posix/pipestream.h>
#include <cxxtools/systemerror.h>
#include <cxxtools/atomicity.h>
#include <cxxtools/arg.h>
#include <list>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <locale>
#include <time.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <cctype>
#include <pwd.h>
#include <grp.h>

log_define("cxxtools.log")

namespace cxxtools
{
  namespace
  {
    void setUserAndGroup(struct passwd* pw, struct group* gr)
    {
      int gret, pret;
      if (gr)
        gret = ::setgid(gr->gr_gid);
      if (pw)
        pret = ::setuid(pw->pw_uid);

      if (gr)
      {
        if (gret == 0)
          log_debug("logging group changed to " << gr->gr_name << '(' << gr->gr_gid << ')');
        else
          log_warn("error changing logging group to " << gr->gr_name << '(' << gr->gr_gid << ')');
      }

      if (pw)
      {
        if (pret == 0)
          log_debug("logging user changed to " << pw->pw_name << '(' << pw->pw_uid << ')');
        else
          log_warn("error changing logging user to " << pw->pw_name << '(' << pw->pw_uid << ')');
      }
    }
  }

  class LoggerImpl : public Logger
  {
      static std::string fname;
      static std::ofstream outfile;
      static Streamcounter counter;
      static Tee tee;
      static net::UdpSender loghost;
      static net::UdpOStream udpmessage;
      static unsigned maxfilesize;
      static unsigned maxbackupindex;
      static posix::Pipestream* pipe;

      static std::string mkfilename(unsigned idx);

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
      static void runLoggerProcess(const std::string& user, const std::string& group);
  };

  std::ostream& LoggerImpl::getAppender()
  {
    if (pipe)
      return *pipe;
    else if (!fname.empty())
    {
      if (!outfile.is_open())
      {
        outfile.clear();
        outfile.open(fname.c_str(), std::ios::out | std::ios::app);
        counter.resetCount(outfile.tellp());
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
      return std::cerr;
  }

  void LoggerImpl::logEnd(std::ostream& appender)
  {
    appender.flush();
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
  Streamcounter LoggerImpl::counter;
  Tee LoggerImpl::tee(LoggerImpl::outfile, LoggerImpl::counter);
  net::UdpSender LoggerImpl::loghost;
  net::UdpOStream LoggerImpl::udpmessage(LoggerImpl::loghost);
  unsigned LoggerImpl::maxfilesize = 0;
  unsigned LoggerImpl::maxbackupindex = 0;
  posix::Pipestream* LoggerImpl::pipe = 0;

  void LoggerImpl::setFile(const std::string& fname_)
  {
    fname = fname_;

    struct stat s;
    int ret = stat(fname_.c_str(), &s);
    counter.resetCount(ret == 0 ? s.st_size : 0);
  }

  void LoggerImpl::setLoghost(const std::string& host, unsigned short int port)
  {
    loghost.connect(host.c_str(), port, true);
  }
  
  void LoggerImpl::runLoggerProcess(const std::string& user, const std::string& group)
  {
    struct passwd * pw = 0;
    if (!user.empty())
    {
      pw = getpwnam(user.c_str());
      if (pw == 0)
        throw std::runtime_error("unknown user \"" + user + "\" in logging configuration");
    }

    struct group * gr = 0;
    if (!group.empty())
    {
      gr = getgrnam(group.c_str());
      if (gr == 0)
        throw std::runtime_error("unknown group \"" + group + "\" in logging configuration");
    }

    pipe = new posix::Pipestream();
    pid_t pid = ::fork();

    if(pid < 0)
      throw SystemError("fork");

    if (pid == 0)
    {
      // 1st child
      pipe->closeWriteFd();
      pid = ::fork();
      if (pid < 0)
        exit(-1);
      if (pid)
        exit(0);  // exit middle process

      // 2nd child

      std::streambuf* in = pipe->rdbuf();
      // set global pipe pointer to 0, so that getAppender do not return
      // that pipe, but skips to the next appender
      pipe = 0;

      setUserAndGroup(pw, gr);

      log_debug("logger process initialized");
      char ich;
      std::ostream& out = getAppender();
      while ((ich = in->snextc()) != std::ios::traits_type::eof())
      {
        char ch = std::ios::traits_type::to_char_type(ich);
        out.rdbuf()->sputc(ch);
        if (ch == '\n')
          getAppender().flush();
      }
      exit(0);
    }
    else
    {
      // parent
      pipe->closeReadFd();
      int status;
      ::waitpid(pid, &status, 0);
      if (WEXITSTATUS(status) != 0)
        throw std::runtime_error("error creating logging process");
    }
  }

  Logger::log_level_type Logger::std_level = LOG_LEVEL_ERROR;
  bool Logger::enabled = false;

  namespace
  {
    ReadWriteMutex Logger_rwmutex;
    Mutex Logger_mutex;
    atomic_t mutexWaitCount = 0;

    class StaticDeinitializer
    {
      public:
        ~StaticDeinitializer()
        {
          Logger::setEnabled(false);
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

    StaticDeinitializer staticDeinitializer;

    typedef std::list<Logger*> loggers_type;

    loggers_type& getBaseLoggers()
    {
      static loggers_type* baseLoggers = 0;
      if (baseLoggers == 0)
      {
        MutexLock lock(Logger_mutex);
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
        MutexLock lock(Logger_mutex);
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

  Logger* Logger::getCategoryLogger(const std::string& category)
  {
    if (!enabled)
      return 0;

    // search existing Logger
    ReadLock rdLock(Logger_rwmutex);

    loggers_type::iterator lower_bound_it = getCacheLoggers().begin();
    while (lower_bound_it != getCacheLoggers().end()
        && (*lower_bound_it)->getCategory() < category)
      ++lower_bound_it;

    if (lower_bound_it != getCacheLoggers().end()
     && (*lower_bound_it)->getCategory() == category)
        return *lower_bound_it;

    // Logger not in list - change to write-lock
    rdLock.unlock();
    WriteLock wrLock(Logger_rwmutex);

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
    WriteLock lock(Logger_rwmutex);

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

  std::ostream& Logger::logentry(std::ostream& out, const char* level, const std::string& category)
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
      date[20] = '\0';
    }

    out << date
        << static_cast<char>('0' + t.tv_usec / 100000 % 10)
        << static_cast<char>('0' + t.tv_usec / 10000 % 10)
        << static_cast<char>('0' + t.tv_usec / 1000 % 10)
        << static_cast<char>('0' + t.tv_usec / 100 % 10)
        << static_cast<char>('0' + t.tv_usec / 10 % 10)
        << " [" << getpid() << '.' << pthread_self() << "] "
        << level << ' '
        << category << " - ";

    return out;
  }

  std::ostream& Logger::logentry(const char* level)
  {
    std::ostream& out = LoggerImpl::getAppender();
    out.clear();
    logentry(out, level, category);
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
        if (!Logger::isEnabled())
          return;

        try
        {
          ScopedAtomicIncrementer inc(mutexWaitCount);
          MutexLock lock(Logger_mutex);

          std::ostream& out(logger->logentry(level));
          out << msg.str() << '\n';

          if (atomicGet(mutexWaitCount) <= 1)
            logger->logEnd(out);
        }
        catch (const std::exception&)
        {
        }
      }

  };

  LogMessage::LogMessage(Logger* logger, const char* level)
    : impl(new LogMessageImpl(logger, level))
    { }

  LogMessage::LogMessage(Logger* logger, Logger::log_level_type level)
    : impl(new LogMessageImpl(logger,
                              level >= Logger::LOG_LEVEL_TRACE ? "TRACE"
                            : level >= Logger::LOG_LEVEL_DEBUG ? "DEBUG"
                            : level >= Logger::LOG_LEVEL_INFO ? "INFO"
                            : level >= Logger::LOG_LEVEL_WARN ? "WARN"
                            : level >= Logger::LOG_LEVEL_ERROR ? "ERROR"
                            : "FATAL"))
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
      if (Logger::isEnabled()
        && l->isEnabled(Logger::LOG_LEVEL_TRACE))
      {
        try
        {
          ScopedAtomicIncrementer inc(mutexWaitCount);
          MutexLock lock(Logger_mutex);
          std::ostream& out = l->logentry("TRACE");
          out << "EXIT " << msg->str() << std::endl;

          if (atomicGet(mutexWaitCount) <= 1)
            l->logEnd(out);
        }
        catch (const std::exception&)
        {
        }
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
    if (msg
      && Logger::isEnabled()
      && l->isEnabled(Logger::LOG_LEVEL_TRACE))
    {
      try
      {
        ScopedAtomicIncrementer inc(mutexWaitCount);
        MutexLock lock(Logger_mutex);
        std::ostream& out = l->logentry("TRACE");
        out << "ENTER " << msg->str() << std::endl;

        if (atomicGet(mutexWaitCount) <= 1)
          l->logEnd(out);
      }
      catch (const std::exception&)
      {
      }
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

bool log_init_cxxtools(const std::string& propertyfilename)
{
  cxxtools::Logger::setEnabled(false);
  cxxtools::getBaseLoggers().clear();

  std::ifstream in(propertyfilename.c_str());
  return log_init_cxxtools(in);
}

bool log_init_cxxtools(std::istream& in)
{
  if (!in)
    return false;

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
    state_disable,
    state_logprocess,
    state_logprocessuser0,
    state_logprocessuser,
    state_logprocessgroup0,
    state_logprocessgroup,
    state_skip
  };
  
  state_type state = state_0;

  char ch;
  std::string token;
  std::string category;
  std::string filename;
  std::string host;
  unsigned short int port = 0;
  unsigned fsize = 0;
  unsigned maxbackupindex = 0;
  bool logprocess = false;
  std::string logprocessuser;
  std::string logprocessgroup;

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
        else if (ch == '=' && (token == "DISABLE" || token == "DISABLED"))
          state = state_disable;
        else if (ch == '=' && token == "LOGPROCESS")
          state = state_logprocess;
        else if (ch == '=' && token == "LOGPROCESSUSER")
          state = state_logprocessuser0;
        else if (ch == '=' && token == "LOGPROCESSGROUP")
          state = state_logprocessgroup0;
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
        else if (ch == '=' && (token == "DISABLE" || token == "DISABLED"))
          state = state_disable;
        else if (ch == '=' && token == "LOGPROCESS")
          state = state_logprocess;
        else if (ch == '=' && token == "LOGPROCESSUSER")
          state = state_logprocessuser0;
        else if (ch == '=' && token == "LOGPROCESSGROUP")
          state = state_logprocessgroup0;
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
          case 'F':
          case 'f': level = cxxtools::Logger::LOG_LEVEL_FATAL; break;
          case 'E':
          case 'e': level = cxxtools::Logger::LOG_LEVEL_ERROR; break;
          case 'W':
          case 'w': level = cxxtools::Logger::LOG_LEVEL_WARN; break;
          case 'I':
          case 'i': level = cxxtools::Logger::LOG_LEVEL_INFO; break;
          case 'D':
          case 'd': level = cxxtools::Logger::LOG_LEVEL_DEBUG; break;
          case 'T':
          case 't': level = cxxtools::Logger::LOG_LEVEL_TRACE; break;
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

      case state_disable:
        if (ch == '1' || ch == 't' || ch == 'T' || ch == 'y' || ch == 'Y')
        {
          cxxtools::Logger::setEnabled(false);
          state = state_skip;
        }
        else if (ch != ' ' && ch != '\t')
          state = (ch == '\n' ? state_0 : state_skip);
        break;

      case state_logprocess:
        if (ch == '1' || ch == 't' || ch == 'T' || ch == 'y' || ch == 'Y')
        {
          logprocess = true;
          state = state_skip;
        }
        else if (ch != ' ' && ch != '\t')
          state = (ch == '\n' ? state_0 : state_skip);
        break;

      case state_logprocessuser0:
        if (ch == '\n')
          state = state_0;
        else if (!std::isspace(ch))
        {
          logprocessuser = ch;
          state = state_logprocessuser;
        }
        break;

      case state_logprocessuser:
        if (ch == '\n')
          state = state_0;
        else if (std::isspace(ch))
          state = state_skip;
        else
          logprocessuser += ch;
        break;

      case state_logprocessgroup0:
        if (ch == '\n')
          state = state_0;
        else if (!std::isspace(ch))
        {
          logprocessgroup = ch;
          state = state_logprocessgroup;
        }
        break;

      case state_logprocessgroup:
        if (ch == '\n')
          state = state_0;
        else if (std::isspace(ch))
          state = state_skip;
        else
          logprocessgroup += ch;
        break;

      case state_skip:
        if (ch == '\n')
          state = state_0;
        break;
    }
  }

  switch (state)
  {
    case state_filename:
      cxxtools::LoggerImpl::setFile(filename);
      break;

    case state_port:
      if (port > 0)
        cxxtools::LoggerImpl::setLoghost(host, port);
      break;

    case state_fsize:
      cxxtools::LoggerImpl::setMaxFileSize(fsize);
      break;

    case state_maxbackupindex:
      cxxtools::LoggerImpl::setMaxBackupIndex(maxbackupindex);
      break;

    default:
      ;
  }

  if (logprocess)
    cxxtools::LoggerImpl::runLoggerProcess(logprocessuser, logprocessgroup);

  cxxtools::reinitializeLoggers();

  return true;
}

bool log_init_cxxtools(int argc, char* argv[])
{
  return log_init_cxxtools(std::string(argv[0]) + ".properties");
}

bool log_init_cxxtools(int argc, char* argv[], char optionchar)
{
  cxxtools::Arg<std::string> pfile(argc, argv, optionchar);
  if (!pfile.isSet())
    return false;

  return log_init_cxxtools(pfile);
}

bool log_init_cxxtools(int argc, char* argv[], const char* option)
{
  cxxtools::Arg<std::string> pfile(argc, argv, option);
  if (!pfile.isSet())
    return false;

  return log_init_cxxtools(pfile);
}

bool log_init_cxxtools()
{
  char* LOGPROPERTIES = ::getenv("LOGPROPERTIES");
  if (LOGPROPERTIES)
    return log_init_cxxtools(LOGPROPERTIES);

  struct stat s;
  if (stat("log.properties", &s) == 0)
    return log_init_cxxtools("log.properties");

  log_init_cxxtools(cxxtools::Logger::LOG_LEVEL_ERROR);
  return false;
}

