/* log.cpp
   Copyright (C) 2003,2004 Tommi Maekitalo

This file is part of cxxtools.

Cxxtools is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Cxxtools is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with cxxtools; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330,
Boston, MA  02111-1307  USA
*/

#include <cxxtools/log.h>

#ifdef CXXTOOLS_USE_LOG4CXX

#include <log4cxx/basicconfigurator.h>
#include <log4cxx/helpers/stringhelper.h>
#include <log4cxx/helpers/exception.h>
#include <log4cxx/xml/domconfigurator.h>
#include <log4cxx/propertyconfigurator.h>
#include <log4cxx/logmanager.h>

void log_init()
{
  char* LOGPROPERTIES = ::getenv("LOGPROPERTIES");
  if (LOGPROPERTIES)
    log_init(LOGPROPERTIES);
  else
  {
    try
    {
      log_init("log4j.properties");
    }
    catch (const log4cxx::helpers::IOException&)
    {
      log4cxx::BasicConfigurator::configure();
    }
  }
}

void log_init(log4cxx::LevelPtr level)
{
  log4cxx::BasicConfigurator::configure();
  log4cxx::LoggerPtr rootLogger = log4cxx::Logger::getRootLogger();
  rootLogger->setLevel(level);
}

void log_init(const std::string& configFileName)
{
#ifdef LOG4CXX_HAVE_XML
  if (log4cxx::helpers::StringHelper::endsWith(configFileName, ".xml"))
  {
    ::log4cxx::xml::DOMConfigurator::configure(configFileName);
  }
  else
#endif
  {
    ::log4cxx::PropertyConfigurator::configure(configFileName);
  }
}

#endif

#ifdef CXXTOOLS_USE_LOG4CPLUS

#include <log4cplus/configurator.h>
#include <log4cplus/consoleappender.h>

void log_init()
{
  char* LOGPROPERTIES = ::getenv("LOGPROPERTIES");
  if (LOGPROPERTIES)
    log_init(LOGPROPERTIES);
  else
  {
    char* LOG4CPLUS = ::getenv("LOG4CPLUS");
    if (LOG4CPLUS)
      log_init(LOG4CPLUS);
    else
      log_init(log4cplus::ERROR_LOG_LEVEL);
  }
}

void log_init(log4cplus::LogLevel level)
{
  log4cplus::SharedAppenderPtr appender(new log4cplus::ConsoleAppender(true, true));
  appender->setName("Main");

  log4cplus::Logger root = log4cplus::Logger::getRoot();
  root.addAppender(appender);
  root.setLogLevel(level);
}

void log_init(const std::string& propertyfilename)
{
  log4cplus::PropertyConfigurator logconfig(propertyfilename);
  logconfig.configure();
}

#endif

#ifdef CXXTOOLS_USE_LOGBUILTIN

#include <cxxtools/log.h>
#include <cxxtools/thread.h>
#include <list>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>

namespace cxxtools
{
  class logger_impl : public logger
  {
      static std::ofstream outfile;

    public:
      logger_impl(const std::string& c, log_level_type l)
        : logger(c, l)
        { }
      std::ostream& getAppender() const;
      static void setFile(const std::string& fname);
  };

  std::ostream& logger_impl::getAppender() const
  {
    return outfile.is_open() ? outfile : std::cout;
  }

  std::ofstream logger_impl::outfile;

  void logger_impl::setFile(const std::string& fname)
  {
    outfile.open(fname.c_str());
  }

  typedef std::list<logger*> loggers_type;
  static loggers_type loggers;
  RWLock logger::rwmutex;
  Mutex logger::mutex;

  log_level_type logger::std_level = LOG_LEVEL_ERROR;

  logger* logger::getLogger(const std::string& category)
  {
    // search existing logger
    RdLock rdLock(rwmutex);

    loggers_type::iterator lower_bound_it = loggers.begin();
    while (lower_bound_it != loggers.end()
        && (*lower_bound_it)->getCategory() < category)
      ++lower_bound_it;

    if (lower_bound_it != loggers.end()
     && (*lower_bound_it)->getCategory() == category)
        return *lower_bound_it;

    // logger not in list - change to write-lock
    rdLock.Unlock();
    WrLock wrLock(rwmutex);

    // we have to do it again after gaining write-lock
    lower_bound_it = loggers.begin();
    while (lower_bound_it != loggers.end()
        && (*lower_bound_it)->getCategory() < category)
      ++lower_bound_it;

    if (lower_bound_it != loggers.end()
     && (*lower_bound_it)->getCategory() == category)
        return *lower_bound_it;

    // logger still not in list, but we have a position to insert

    // search best-fit logger
    std::string::size_type best_len = 0;
    log_level_type best_level = std_level;

    for (loggers_type::iterator it = loggers.begin();
         it != loggers.end(); ++it)
    {
      if ((*it)->getCategory().size() > best_len
        && (*it)->getCategory().size() < category.size()
        && category.at((*it)->getCategory().size()) == '.'
        && category.compare(0, (*it)->getCategory().size(), (*it)->getCategory()) == 0)
      {
        best_len = (*it)->getCategory().size();
        // update log-level
        best_level = (*it)->getLogLevel();
      }
    }

    // insert the new logger in list and return pointer to the new list-element
    return *(loggers.insert(lower_bound_it, new logger_impl(category, best_level)));
  }

  logger* logger::setLevel(const std::string& category, log_level_type l)
  {
    WrLock lock(rwmutex);

    // search for existing logger
    loggers_type::iterator it = loggers.begin();
    while (it != loggers.end()
        && (*it)->getCategory() < category)
      ++it;

    if (it == loggers.end() || (*it)->getCategory() != category)
    {
      // logger not found - create new
      it = loggers.insert(it, new logger_impl(category, l));
    }
    else
      (*it)->setLogLevel(l); // logger found - set level

    // return pointer to object in list
    return *it;
  }

  std::ostream& logger::logentry(const char* level) const
  {
    struct timeval t;
    struct tm tt;
    gettimeofday(&t, 0);
    time_t sec = static_cast<time_t>(t.tv_sec);
    localtime_r(&sec, &tt);

    std::ostream& out = getAppender();
    out << 1900 + tt.tm_year << '-'
        << ((tt.tm_mon + 1) / 10) << ((tt.tm_mon + 1) % 10) << '-'
        << (tt.tm_mday / 10) << (tt.tm_mday % 10) << ' '
        << (tt.tm_hour / 10) << (tt.tm_hour % 10) << ':'
        << (tt.tm_min / 10) << (tt.tm_min % 10) << ':'
        << (tt.tm_sec / 10)
        << (tt.tm_sec % 10 + t.tv_usec / 1e6)
        << " [" << pthread_self() << "] "
        << level << ' '
        << category << " - ";

    return out;
  }

  log_tracer::log_tracer(logger* l_, const std::string& msg_)
    : l(l_),
      msg(msg_)
  {
    if (l->isEnabled(LOG_LEVEL_TRACE))
    {
      cxxtools::MutexLock lock(cxxtools::logger::mutex);
      l->logentry("ENTER")
        << msg << std::endl;
    }
  }

  log_tracer::~log_tracer()
  {
    if (l->isEnabled(LOG_LEVEL_TRACE))
    {
      cxxtools::MutexLock lock(cxxtools::logger::mutex);
      l->logentry("EXIT")
        << msg << std::endl;
    }
  }
}

void log_init(const std::string& propertyfilename)
{
  log_init(cxxtools::LOG_LEVEL_ERROR);

  std::ifstream in(propertyfilename.c_str());

  enum state_type {
    state_0,
    state_token,
    state_category,
    state_level,
    state_rootlevel,
    state_filename,
    state_skip
  };
  
  state_type state = state_0;

  char ch;
  std::string token;
  std::string category;
  std::string filename;
  cxxtools::log_level_type level;
  while (in.get(ch))
  {
    switch (state)
    {
      case state_0:
        if (std::isalnum(ch))
        {
          token = ch;
          state = state_token;
        }
        else if (!std::isspace(ch))
          state = state_skip;
        break;

      case state_token:
        if (ch == '.')
        {
          if (token == "logger")
            state = state_category;
          else
          {
            token.clear();
            state = state_token;
          }
        }
        else if (ch == '=' && token == "rootLogger")
          state = state_rootlevel;
        else if (ch == '=' && token == "File")
          state = state_filename;
        else if (std::isalnum(ch))
          token += ch;
        else
        {
          token.clear();
          state = state_skip;
        }
        break;

      case state_category:
        if (std::isalnum(ch) || ch == '.')
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
        switch (ch)
        {
          case 'F': level = cxxtools::LOG_LEVEL_FATAL; break;
          case 'E': level = cxxtools::LOG_LEVEL_ERROR; break;
          case 'W': level = cxxtools::LOG_LEVEL_WARN; break;
          case 'I': level = cxxtools::LOG_LEVEL_INFO; break;
          case 'D': level = cxxtools::LOG_LEVEL_DEBUG; break;
          case 'T': level = cxxtools::LOG_LEVEL_TRACE; break;
          default: level = cxxtools::logger::getStdLevel(); break;
        }
        if (state == state_rootlevel)
          cxxtools::logger::setRootLevel(level);
        else
          cxxtools::logger::setLevel(category, level);
        category.clear();
        token.clear();
        state = state_skip;
        break;

      case state_filename:
        if (ch == '\n')
        {
          cxxtools::logger_impl::setFile(filename);
          token.clear();
          filename.clear();
          state = state_0;
        }
        else
          filename += ch;
        break;

      case state_skip:
        if (ch == '\n')
          state = state_0;
        break;
    }
  }
}

void log_init()
{
  char* LOGPROPERTIES = ::getenv("LOGPROPERTIES");
  if (LOGPROPERTIES)
    log_init(LOGPROPERTIES);
  else
    log_init(cxxtools::LOG_LEVEL_ERROR);
}

#endif
