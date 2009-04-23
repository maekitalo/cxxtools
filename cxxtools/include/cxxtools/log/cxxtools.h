/*
 * Copyright (C) 2003, Tommi Maekitalo
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

#ifndef CXXTOOLS_LOG_CXXTOOLS_H
#define CXXTOOLS_LOG_CXXTOOLS_H

#include <string>
#include <cxxtools/mutex.h>
#include <iostream>
#include <sstream>

#define log_xxxx_enabled(level)   \
  (getLogger() != 0 && getLogger()->isEnabled(::cxxtools::Logger::LOG_LEVEL_ ## level))

#define log_xxxx(level, expr)   \
  do { \
    ::cxxtools::Logger* _cxxtools_logger = getLogger(); \
    if (_cxxtools_logger != 0 && _cxxtools_logger->isEnabled(::cxxtools::Logger::LOG_LEVEL_ ## level)) \
    { \
      ::cxxtools::LogMessage _cxxtools_logMessage(_cxxtools_logger, #level); \
      _cxxtools_logMessage.out() << expr; \
      _cxxtools_logMessage.flush(); \
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
    static cxxtools::Logger* logger = 0; \
    if (logger == 0 && ::cxxtools::Logger::isEnabled()) \
      logger = ::cxxtools::Logger::getCategoryLogger(category); \
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
      static bool enabled;

    protected:
      Logger(const std::string& c, log_level_type l)
        : category(c), level(l)
        { }
      virtual ~Logger()  { }

    public:
      static Logger* getCategoryLogger(const std::string& category);
      static void setRootLevel(log_level_type l)
        { std_level = l; }
      static log_level_type getStdLevel()
        { return std_level; }
      static Logger* setLevel(const std::string& category, log_level_type l);
      static void setEnabled(bool sw = true)   { enabled = sw; }
      static bool isEnabled()
        { return enabled; }

      bool isEnabled(log_level_type l) const
        { return level >= l && enabled; }
      const std::string& getCategory() const
        { return category; }
      log_level_type getLogLevel() const
        { return level; }
      void setLogLevel(log_level_type l)
        { level = l; }
      /// prints the start of a longentry into the outstream
      static std::ostream& logentry(std::ostream& out, const char* level, const std::string& category);
      /// prints the start of a longentry and returns the appender
      std::ostream& logentry(const char* level);
      virtual void logEnd(std::ostream& appender) = 0;

      static ReadWriteMutex rwmutex;
      static Mutex mutex;
  };

  class LogMessage
  {
      class LogMessageImpl;
      LogMessageImpl* impl;

    public:
      LogMessage(Logger* logger, const char* level);
      LogMessage(Logger* logger, Logger::log_level_type level);
      ~LogMessage();

      std::ostream& out();
      void flush();
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
