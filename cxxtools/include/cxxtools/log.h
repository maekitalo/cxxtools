/* cxxtools/log.h
   Copyright (C) 2003, 2004 Tommi Maekitalo

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

#ifndef CXXTOOLS_LOG_H
#define CXXTOOLS_LOG_H

#include <cxxtools/logfwd.h>

#ifdef CXXTOOLS_USE_LOG4CXX

#include <log4cxx/logger.h>
#include <sstream>

#define CXXTOOLS_LOG(level, expr) \
  do { \
    if (getLogger()->isEnabledFor(::log4cxx::Level::level)) \
    { \
      std::ostringstream msg; \
      msg << expr; \
      getLogger()->log( \
        ::log4cxx::Level::level, \
        msg.str(), \
        LOG4CXX_LOCATION); \
    } \
  } while (false)

#define log_fatal(expr)  CXXTOOLS_LOG(FATAL, expr)
#define log_error(expr)  CXXTOOLS_LOG(ERROR, expr)
#define log_warn(expr)   CXXTOOLS_LOG(WARN, expr)
#define log_info(expr)   CXXTOOLS_LOG(INFO, expr)
#define log_debug(expr)  CXXTOOLS_LOG(DEBUG, expr)

#define log_define(category) \
  static inline log4cxx::LoggerPtr& getLogger()   \
  { \
    static log4cxx::LoggerPtr l = ::log4cxx::Logger::getLogger(category); \
    return l; \
  }

#endif

#ifdef CXXTOOLS_USE_LOG4CPLUS

#include <log4cplus/logger.h>

#define log_fatal(expr)  do { LOG4CPLUS_FATAL(getLogger(), expr) } while(false)
#define log_error(expr)  do { LOG4CPLUS_ERROR(getLogger(), expr) } while(false)
#define log_warn(expr)   do { LOG4CPLUS_WARN(getLogger(), expr) } while(false)
#define log_info(expr)   do { LOG4CPLUS_INFO(getLogger(), expr) } while(false)
#define log_debug(expr)  do { LOG4CPLUS_DEBUG(getLogger(), expr) } while(false)

#define log_trace(event) do { LOG4CPLUS_TRACE_METHOD(getLogger(), event) } while(false)

#define log_define(category)   \
  static inline log4cplus::Logger& getLogger()   \
  {  \
    static log4cplus::Logger tntlogger = log4cplus::Logger::getInstance(category);  \
    return tntlogger;  \
  }

#endif

#ifdef CXXTOOLS_USE_LOGBUILTIN

#include <string>
#include <cxxtools/thread.h>
#include <iostream>

#define log_(level, expr)   \
  do { \
    cxxtools::logger* logger = getLogger(); \
    if (logger->isEnabled(::cxxtools::logger::LOG_LEVEL_ ## level)) \
    { \
      cxxtools::MutexLock lock(cxxtools::logger::mutex); \
      logger->logentry(#level) \
        << expr << std::endl; \
    } \
  } while (false)

#define log_fatal(expr)     log_(FATAL, expr)
#define log_error(expr)     log_(ERROR, expr)
#define log_warn(expr)      log_(WARN, expr)
#define log_info(expr)      log_(INFO, expr)
#define log_debug(expr)     log_(DEBUG, expr)

#define log_define(category) \
  static inline ::cxxtools::logger* getLogger()   \
  {  \
    static cxxtools::logger* logger = 0; \
    if (logger == 0) \
      logger = ::cxxtools::logger::getLogger(category); \
    return logger; \
  }

namespace cxxtools
{
  class logger
  {
    public:
      typedef enum {
        LOG_LEVEL_FATAL = 0,
        LOG_LEVEL_ERROR = 100,
        LOG_LEVEL_WARN  = 200,
        LOG_LEVEL_INFO  = 300,
        LOG_LEVEL_DEBUG = 400,
        LOG_LEVEL_TRACE = 500
      } log_level_type;

    private:
      std::string category;
      log_level_type level;
      static log_level_type std_level;

    protected:
      logger(const std::string& c, log_level_type l)
        : category(c), level(l)
        { }
      virtual ~logger()  { }

    public:
      static logger* getLogger(const std::string& category);
      static void setRootLevel(log_level_type l)
        { std_level = l; }
      static log_level_type getStdLevel()
        { return std_level; }
      static logger* setLevel(const std::string& category, log_level_type l);

      bool isEnabled(log_level_type l)
        { return level >= l; }
      const std::string& getCategory() const
        { return category; }
      log_level_type getLogLevel() const
        { return level; }
      void setLogLevel(log_level_type l)
        { level = l; }
      virtual std::ostream& getAppender() const = 0;
      std::ostream& logentry(const char* level) const;

      static RWLock rwmutex;
      static Mutex mutex;
  };
}

#endif

#ifdef CXXTOOLS_DISABLE_LOG

#define log_fatal(expr)
#define log_error(expr)
#define log_warn(expr)
#define log_info(expr)
#define log_debug(expr)
#define log_define(category)

#endif

#endif // LOG_H
