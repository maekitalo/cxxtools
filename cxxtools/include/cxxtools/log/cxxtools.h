/* cxxtools/log_cxxtools.h
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

#ifndef CXXTOOLS_LOG_CXXTOOLS_H
#define CXXTOOLS_LOG_CXXTOOLS_H

#include <string>
#include <cxxtools/thread.h>
#include <iostream>

#define log_xxxx_enabled(level)   \
  getLogger()->isEnabled(::cxxtools::Logger::LOG_LEVEL_ ## level)

#define log_xxxx(level, expr)   \
  do { \
    cxxtools::Logger* logger = getLogger(); \
    if (getLogger()->isEnabled(::cxxtools::Logger::LOG_LEVEL_ ## level)) \
    { \
      cxxtools::MutexLock lock(cxxtools::Logger::mutex); \
      logger->logentry(#level) \
        << expr << std::endl; \
    } \
  } while (false)

#define log_fatal_enabled()     log_xxxx_enabled(FATAL)
#define log_error_enabled()     log_xxxx_enabled(ERROR)
#define log_warn_enabled()      log_xxxx_enabled(WARN)
#define log_info_enabled()      log_xxxx_enabled(INFO)
#define log_debug_enabled()     log_xxxx_enabled(DEBUG)
#define log_trace_enabled()     log_xxxx_enabled(TRACE)

#define log_fatal(expr)     log_xxxx(FATAL, expr)
#define log_error(expr)     log_xxxx(ERROR, expr)
#define log_warn(expr)      log_xxxx(WARN, expr)
#define log_info(expr)      log_xxxx(INFO, expr)
#define log_debug(expr)     log_xxxx(DEBUG, expr)
#define log_trace(expr)     \
  ::cxxtools::LogTracer tracer ## __LINE__ (getLogger());  \
  if (log_trace_enabled()) \
  { \
    tracer ## __LINE__ .logentry() << expr;  \
    tracer ## __LINE__ .enter();  \
  }

#define log_define(category) \
  static inline ::cxxtools::Logger* getLogger()   \
  {  \
    static cxxtools::Logger* logger = ::cxxtools::Logger::getLogger(category); \
    return logger; \
  }

namespace cxxtools
{
  class Logger
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
      Logger(const std::string& c, log_level_type l)
        : category(c), level(l)
        { }
      virtual ~Logger()  { }

    public:
      static Logger* getLogger(const std::string& category);
      static void setRootLevel(log_level_type l)
        { std_level = l; }
      static log_level_type getStdLevel()
        { return std_level; }
      static Logger* setLevel(const std::string& category, log_level_type l);

      bool isEnabled(log_level_type l)
        { return level >= l; }
      const std::string& getCategory() const
        { return category; }
      log_level_type getLogLevel() const
        { return level; }
      void setLogLevel(log_level_type l)
        { level = l; }
      virtual std::ostream& getAppender() = 0;
      virtual void endMessage() const  { }
      std::ostream& logentry(const char* level);

      static RWLock rwmutex;
      static Mutex mutex;
  };

  class LogTracer
  {
      Logger* l;
      std::ostringstream* msg;

    public:
      LogTracer(Logger* l_)
        : l(l_), msg(0)
      { }
      ~LogTracer();

      std::ostream& logentry();
      void enter();
  };
}

#endif // LOG_CXXTOOLS_H
