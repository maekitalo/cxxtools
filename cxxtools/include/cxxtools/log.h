/* cxxtools/log.h
   Copyright (C) 2003 Tommi Mae¤kitalo

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

#define log_fatal(expr)   do { LOG4CXX_FATAL(getLogger(), expr); } while(false)
#define log_error(expr)   do { LOG4CXX_ERROR(getLogger(), expr); } while(false)
#define log_warn(expr)    do { LOG4CXX_WARN(getLogger(), expr); } while(false)
#define log_info(expr)    do { LOG4CXX_INFO(getLogger(), expr); } while(false)
#define log_debug(expr)   do { LOG4CXX_DEBUG(getLogger(), expr); } while(false)
#define log_trace(event)  ::cxxtools::log4cxx_tracer log4cxx_trace_logger(getLogger(), event)

#define log_init_fatal()   log_init(::log4cxx::Level::FATAL)
#define log_init_error()   log_init(::log4cxx::Level::ERROR)
#define log_init_warn()    log_init(::log4cxx::Level::WARN)
#define log_init_info()    log_init(::log4cxx::Level::INFO)
#define log_init_debug()   log_init(::log4cxx::Level::DEBUG)
#define log_init_trace()   log_init(::log4cxx::Level::TRACE)

void log_init();
void log_init(::log4cxx::LevelPtr level);
void log_init(const std::string& filename);
void log_init(const char* filename)
{ log_init(std::string(filename)); }

namespace cxxtools
{
  class log4cxx_tracer
  {
      ::log4cxx::LoggerPtr logger;
      std::string event;

    public:
      log4cxx_tracer(::log4cxx::LoggerPtr _logger, const std::string& _event)
        : logger(_logger),
          event(_event)
        { LOG4CXX_DEBUG(logger, "ENTER " + event); }
      ~log4cxx_tracer()
        { LOG4CXX_DEBUG(logger, "EXIT " + event); }
  };
}

#define log_define(category) \
  log4cxx::LoggerPtr getLogger()   \
  {  \
    static const std::string log_category = category;  \
    return ::log4cxx::Logger::getLogger(log_category); \
  }

#define log_define_static(category) \
  static log_define(category)

#define log_define_class(classname, category) \
  log4cxx::LoggerPtr classname::getLogger()   \
  {  \
    static const std::string log_category = category;  \
    return ::log4cxx::Logger::getLogger(log_category); \
  }

#endif

#ifdef CXXTOOLS_USE_LOG4CPLUS

#include <log4cplus/logger.h>

#define log_fatal(expr)  do { LOG4CPLUS_FATAL(getLogger(), expr) } while(false)
#define log_error(expr)  do { LOG4CPLUS_ERROR(getLogger(), expr) } while(false)
#define log_warn(expr)   do { LOG4CPLUS_WARN(getLogger(), expr) } while(false)
#define log_info(expr)   do { LOG4CPLUS_INFO(getLogger(), expr) } while(false)
#define log_debug(expr)  do { LOG4CPLUS_DEBUG(getLogger(), expr) } while(false)

#define log_trace(event)  do { LOG4CPLUS_TRACE_METHOD(getLogger(), event) } while(false)

#define log_init_fatal()   log_init(log4cplus::FATAL_LOG_LEVEL)
#define log_init_error()   log_init(log4cplus::ERROR_LOG_LEVEL)
#define log_init_warn()    log_init(log4cplus::WARN_LOG_LEVEL)
#define log_init_info()    log_init(log4cplus::INFO_LOG_LEVEL)
#define log_init_debug()   log_init(log4cplus::DEBUG_LOG_LEVEL)
#define log_init_trace()   log_init(log4cplus::TRACE_LOG_LEVEL)

void log_init();
void log_init(log4cplus::LogLevel level);
void log_init(const std::string& propertyfilename);

#define log_define(category)   \
  log4cplus::Logger getLogger()   \
  {  \
    static const std::string log_category = category;  \
    log4cplus::Logger tntlogger = log4cplus::Logger::getInstance(log_category);  \
    return tntlogger;  \
  }

#define log_define_static(category) \
  static log_define(category)

#define log_define_class(classname, category)   \
  log4cplus::Logger classname::getLogger()   \
  {  \
    static const std::string log_category = category;  \
    log4cplus::Logger tntlogger = log4cplus::Logger::getInstance(log_category);  \
    return tntlogger;  \
  }

#endif

#ifdef CXXTOOLS_USE_LOGSTDOUT

#include <string>

namespace cxxtools
{
  typedef enum {
    LOG_LEVEL_FATAL = 0,
    LOG_LEVEL_ERROR = 100,
    LOG_LEVEL_WARN  = 200,
    LOG_LEVEL_INFO  = 300,
    LOG_LEVEL_DEBUG = 400,
    LOG_LEVEL_TRACE = 500
  } log_level_type;

  extern log_level_type log_level;
}

#define log_(level, expr)   do { if (cxxtools::log_level >= level) { std::cout << expr; } } while (false)
#define log_fatal(expr)     log_(LOG_LEVEL_FATAL expr)
#define log_error(expr)     log_(LOG_LEVEL_ERROR, expr)
#define log_warn(expr)      log_(LOG_LEVEL_WARN, expr)
#define log_info(expr)      log_(LOG_LEVEL_INFO, expr)
#define log_debug(expr)     log_(LOG_LEVEL_DEBUG, expr)
#define log_trace(event)    log_(LOG_LEVEL_TRACE, expr)

#define log_define(category)
#define log_define_static(category)
#define log_define_class(classname, category)

#define log_init_fatal()   log_init(FATAL_LOG_LEVEL)
#define log_init_error()   log_init(ERROR_LOG_LEVEL)
#define log_init_warn()    log_init(WARN_LOG_LEVEL)
#define log_init_info()    log_init(INFO_LOG_LEVEL)
#define log_init_debug()   log_init(DEBUG_LOG_LEVEL)
#define log_init_trace()   log_init(TRACE_LOG_LEVEL)

inline void log_init(cxxtools::log_level_type level = cxxtools::LOG_LEVEL_ERROR)
{
  cxxtools::log_level = level;
}

inline void log_init(const std::string& propertyfilename)
{ }

#endif

#endif // LOG_H
