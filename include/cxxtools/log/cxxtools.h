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
#include <iostream>

#define _cxxtools_log_enabled(impl, level)   \
  (getLogger ## impl() != 0 && getLogger ## impl()->isEnabled(::cxxtools::Logger::level))

#define _cxxtools_log(impl, level, displaylevel, expr)   \
  do { \
    ::cxxtools::Logger* _cxxtools_logger = getLogger ## impl(); \
    if (_cxxtools_logger != 0 && _cxxtools_logger->isEnabled(::cxxtools::Logger::level)) \
    { \
      ::cxxtools::LogMessage _cxxtools_logMessage(_cxxtools_logger, displaylevel); \
      _cxxtools_logMessage.out() << expr; \
      _cxxtools_logMessage.finish(); \
    } \
  } while (false)

#define _cxxtools_log_if(impl, level, displaylevel, cond, expr)   \
  do { \
    ::cxxtools::Logger* _cxxtools_logger = getLogger ## impl(); \
    if (_cxxtools_logger != 0 && _cxxtools_logger->isEnabled(::cxxtools::Logger::level) && (cond)) \
    { \
      ::cxxtools::LogMessage _cxxtools_logMessage(_cxxtools_logger, displaylevel); \
      _cxxtools_logMessage.out() << expr; \
      _cxxtools_logMessage.finish(); \
    } \
  } while (false)

#define log_fatal_enabled()     _cxxtools_log_enabled(_default, LOG_LEVEL_FATAL)
#define log_error_enabled()     _cxxtools_log_enabled(_default, LOG_LEVEL_ERROR)
#define log_warn_enabled()      _cxxtools_log_enabled(_default, LOG_LEVEL_WARN)
#define log_info_enabled()      _cxxtools_log_enabled(_default, LOG_LEVEL_INFO)
#define log_debug_enabled()     _cxxtools_log_enabled(_default, LOG_LEVEL_DEBUG)
#define log_trace_enabled()     _cxxtools_log_enabled(_default, LOG_LEVEL_TRACE)

#define log_fatal_to_enabled(impl)     _cxxtools_log_enabled(impl, LOG_LEVEL_FATAL)
#define log_error_to_enabled(impl)     _cxxtools_log_enabled(impl, LOG_LEVEL_ERROR)
#define log_warn_to_enabled(impl)      _cxxtools_log_enabled(impl, LOG_LEVEL_WARN)
#define log_info_to_enabled(impl)      _cxxtools_log_enabled(impl, LOG_LEVEL_INFO)
#define log_debug_to_enabled(impl)     _cxxtools_log_enabled(impl, LOG_LEVEL_DEBUG)
#define log_trace_to_enabled(impl)     _cxxtools_log_enabled(impl, LOG_LEVEL_TRACE)

#define log_fatal(expr)     _cxxtools_log(_default, LOG_LEVEL_FATAL, "FATAL", expr)
#define log_error(expr)     _cxxtools_log(_default, LOG_LEVEL_ERROR, "ERROR", expr)
#define log_warn(expr)      _cxxtools_log(_default, LOG_LEVEL_WARN, "WARN", expr)
#define log_info(expr)      _cxxtools_log(_default, LOG_LEVEL_INFO, "INFO", expr)
#define log_debug(expr)     _cxxtools_log(_default, LOG_LEVEL_DEBUG, "DEBUG", expr)

#define log_fatal_to(impl, expr)     _cxxtools_log(impl, LOG_LEVEL_FATAL, "FATAL", expr)
#define log_error_to(impl, expr)     _cxxtools_log(impl, LOG_LEVEL_ERROR, "ERROR", expr)
#define log_warn_to(impl, expr)      _cxxtools_log(impl, LOG_LEVEL_WARN, "WARN", expr)
#define log_info_to(impl, expr)      _cxxtools_log(impl, LOG_LEVEL_INFO, "INFO", expr)
#define log_debug_to(impl, expr)     _cxxtools_log(impl, LOG_LEVEL_DEBUG, "DEBUG", expr)

#define log_fatal_if(cond, expr)     _cxxtools_log_if(_default, LOG_LEVEL_FATAL, "FATAL", cond, expr)
#define log_error_if(cond, expr)     _cxxtools_log_if(_default, LOG_LEVEL_ERROR, "ERROR", cond, expr)
#define log_warn_if(cond, expr)      _cxxtools_log_if(_default, LOG_LEVEL_WARN, "WARN", cond, expr)
#define log_info_if(cond, expr)      _cxxtools_log_if(_default, LOG_LEVEL_INFO, "INFO", cond, expr)
#define log_debug_if(cond, expr)     _cxxtools_log_if(_default, LOG_LEVEL_DEBUG, "DEBUG", cond, expr)

#define log_fatal_to_if(impl, cond, expr)     _cxxtools_log_if(impl, LOG_LEVEL_FATAL, "FATAL", cond, expr)
#define log_error_to_if(impl, cond, expr)     _cxxtools_log_if(impl, LOG_LEVEL_ERROR, "ERROR", cond, expr)
#define log_warn_to_if(impl, cond, expr)      _cxxtools_log_if(impl, LOG_LEVEL_WARN, "WARN", cond, expr)
#define log_info_to_if(impl, cond, expr)      _cxxtools_log_if(impl, LOG_LEVEL_INFO, "INFO", cond, expr)
#define log_debug_to_if(impl, cond, expr)     _cxxtools_log_if(impl, LOG_LEVEL_DEBUG, "DEBUG", cond, expr)

#define log_trace_to(impl, expr)     \
  ::cxxtools::LogTracer _cxxtools_tracer;  \
  do { \
    ::cxxtools::Logger* _cxxtools_logger = getLogger ## impl(); \
    if (_cxxtools_logger != 0 && _cxxtools_logger->isEnabled(::cxxtools::Logger::LOG_LEVEL_TRACE)) \
    { \
      _cxxtools_tracer.setLogger(_cxxtools_logger); \
      _cxxtools_tracer.out() << expr;  \
      _cxxtools_tracer.enter();  \
    } \
  } while (false)

#define log_trace(expr)     log_trace_to(_default, expr)

#define log_define_instance(instance, category) \
  static ::cxxtools::Logger* getLogger ## instance()   \
  {  \
    static cxxtools::Logger* logger = 0; \
    if (!::cxxtools::LogManager::isEnabled()) \
      return 0; \
    if (logger == 0) \
      logger = ::cxxtools::LogManager::getInstance().getLogger(category); \
    return logger; \
  }

#define log_define(category) log_define_instance(_default, category)

#define log_init_cxxtools  ::cxxtools::LogManager::logInit
#define log_init  log_init_cxxtools

namespace cxxtools
{
  class SerializationInfo;

  //////////////////////////////////////////////////////////////////////
  //
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

      Logger(const Logger&);
      Logger& operator=(const Logger&);

    public:
      Logger(const std::string& c, log_level_type l)
        : category(c), level(l)
        { }

      bool isEnabled(log_level_type l) const
        { return level >= l; }
      const std::string& getCategory() const
        { return category; }
      log_level_type getLogLevel() const
        { return level; }
      void setLogLevel(log_level_type level_)
        { level = level_; }
  };

  //////////////////////////////////////////////////////////////////////
  //
  class LogConfiguration
  {
    public:
      class Impl;

    private:
      friend class Impl;
      Impl* _impl;

    public:
      typedef Logger::log_level_type log_level_type;

      LogConfiguration();
      LogConfiguration(const LogConfiguration&);
      LogConfiguration& operator=(const LogConfiguration&);

      ~LogConfiguration();

      Impl* impl()             { return _impl; }
      const Impl* impl() const { return _impl; }

      log_level_type rootLevel() const;
      log_level_type logLevel(const std::string& category) const;

      // setter
      void setRootLevel(log_level_type level);
      void setLogLevel(const std::string& category, log_level_type level);
      void setFile(const std::string& fname);
      void setFile(const std::string& fname, unsigned maxfilesize, unsigned maxbackupindex);
      void setLoghost(const std::string& host, unsigned short port, bool broadcast = false);
      void setStdout();
      void setStderr();
  };

  void operator>>= (const SerializationInfo& si, LogConfiguration& logConfiguration);
  void operator<<= (SerializationInfo& si, const LogConfiguration& logConfiguration);

  //////////////////////////////////////////////////////////////////////
  //
  class LogManager
  {
    public:
      class Impl;

      friend class Impl;

      Impl* _impl;
      LogManager();
      static bool _enabled;

      LogManager(const LogManager&);
      LogManager& operator=(const LogManager&);

    public:
      ~LogManager();

      Impl* impl()              { return _impl; }
      const Impl* impl() const  { return _impl; }

      static LogManager& getInstance();
      static void logInit();
      static void logInit(const std::string& fname);
      static void logInit(const SerializationInfo& si);
      static void logInit(const LogConfiguration& config);

      void configure(const LogConfiguration& config);
      LogConfiguration getLogConfiguration() const;

      Logger* getLogger(const std::string& category);
      static bool isEnabled()
      { return _enabled; }

      Logger::log_level_type rootLevel() const;
      Logger::log_level_type logLevel(const std::string& category) const;
  };

  //////////////////////////////////////////////////////////////////////
  //
  class LogMessage
  {
    public:
      class Impl;

    private:
      Impl* _impl;

      LogMessage(const LogMessage&);
      LogMessage& operator=(const LogMessage&);

    public:
      LogMessage(Logger* logger, const char* level);
      LogMessage(Logger* logger, Logger::log_level_type level);
      ~LogMessage();

      Impl* impl()             { return _impl; }
      const Impl* impl() const { return _impl; }

      std::ostream& out();
      std::string str() const;

      void finish();
  };

  //////////////////////////////////////////////////////////////////////
  //
  class LogTracer
  {
    public:
      class Impl;

    private:
      Impl* _impl;

      LogTracer(const LogTracer&);
      LogTracer& operator=(const LogTracer&);

    public:
      LogTracer();
      ~LogTracer();

      Impl* impl()             { return _impl; }
      const Impl* impl() const { return _impl; }

      void setLogger(Logger* l);
      std::ostream& out();
      void enter();
      void exit();
  };

}

#endif // LOG_CXXTOOLS_H
