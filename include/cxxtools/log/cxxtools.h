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

#define log_fatal_enabled()     _cxxtools_log_enabled(_default, LOG_FATAL)
#define log_error_enabled()     _cxxtools_log_enabled(_default, LOG_ERROR)
#define log_warn_enabled()      _cxxtools_log_enabled(_default, LOG_WARN)
#define log_info_enabled()      _cxxtools_log_enabled(_default, LOG_INFO)
#define log_debug_enabled()     _cxxtools_log_enabled(_default, LOG_DEBUG)
#define log_fine_enabled()      _cxxtools_log_enabled(_default, LOG_FINE)
#define log_finer_enabled()     _cxxtools_log_enabled(_default, LOG_FINER)
#define log_finest_enabled()    _cxxtools_log_enabled(_default, LOG_FINEST)
#define log_trace_enabled()     _cxxtools_log_enabled(_default, LOG_TRACE)

#define log_fatal_to_enabled(impl)     _cxxtools_log_enabled(impl, LOG_FATAL)
#define log_error_to_enabled(impl)     _cxxtools_log_enabled(impl, LOG_ERROR)
#define log_warn_to_enabled(impl)      _cxxtools_log_enabled(impl, LOG_WARN)
#define log_info_to_enabled(impl)      _cxxtools_log_enabled(impl, LOG_INFO)
#define log_debug_to_enabled(impl)     _cxxtools_log_enabled(impl, LOG_DEBUG)
#define log_fine_to_enabled(impl)      _cxxtools_log_enabled(impl, LOG_FINE)
#define log_finer_to_enabled(impl)     _cxxtools_log_enabled(impl, LOG_FINER)
#define log_finest_to_enabled(impl)    _cxxtools_log_enabled(impl, LOG_FINEST)
#define log_trace_to_enabled(impl)     _cxxtools_log_enabled(impl, LOG_TRACE)

#define log_fatal(expr)     _cxxtools_log(_default, LOG_FATAL, "FATAL", expr)
#define log_error(expr)     _cxxtools_log(_default, LOG_ERROR, "ERROR", expr)
#define log_warn(expr)      _cxxtools_log(_default, LOG_WARN, "WARN", expr)
#define log_info(expr)      _cxxtools_log(_default, LOG_INFO, "INFO", expr)
#define log_debug(expr)     _cxxtools_log(_default, LOG_DEBUG, "DEBUG", expr)
#define log_fine(expr)      _cxxtools_log(_default, LOG_FINE, "FINE", expr)
#define log_finer(expr)     _cxxtools_log(_default, LOG_FINER, "FINER", expr)
#define log_finest(expr)    _cxxtools_log(_default, LOG_FINEST, "FINEST", expr)

#define log_fatal_to(impl, expr)     _cxxtools_log(impl, LOG_FATAL, "FATAL", expr)
#define log_error_to(impl, expr)     _cxxtools_log(impl, LOG_ERROR, "ERROR", expr)
#define log_warn_to(impl, expr)      _cxxtools_log(impl, LOG_WARN, "WARN", expr)
#define log_info_to(impl, expr)      _cxxtools_log(impl, LOG_INFO, "INFO", expr)
#define log_debug_to(impl, expr)     _cxxtools_log(impl, LOG_DEBUG, "DEBUG", expr)
#define log_fine_to(impl, expr)      _cxxtools_log(impl, LOG_FINE, "FINE", expr)
#define log_finer_to(impl, expr)     _cxxtools_log(impl, LOG_FINER, "FINER", expr)
#define log_finest_to(impl, expr)    _cxxtools_log(impl, LOG_FINEST, "FINEST", expr)

#define log_fatal_if(cond, expr)     _cxxtools_log_if(_default, LOG_FATAL, "FATAL", cond, expr)
#define log_error_if(cond, expr)     _cxxtools_log_if(_default, LOG_ERROR, "ERROR", cond, expr)
#define log_warn_if(cond, expr)      _cxxtools_log_if(_default, LOG_WARN, "WARN", cond, expr)
#define log_info_if(cond, expr)      _cxxtools_log_if(_default, LOG_INFO, "INFO", cond, expr)
#define log_debug_if(cond, expr)     _cxxtools_log_if(_default, LOG_DEBUG, "DEBUG", cond, expr)
#define log_fine_if(cond, expr)      _cxxtools_log_if(_default, LOG_FINE, "FINE", cond, expr)
#define log_finer_if(cond, expr)     _cxxtools_log_if(_default, LOG_FINER, "FINER", cond, expr)
#define log_finest_if(cond, expr)    _cxxtools_log_if(_default, LOG_FINEST, "FINEST", cond, expr)

#define log_fatal_to_if(impl, cond, expr)     _cxxtools_log_if(impl, LOG_FATAL, "FATAL", cond, expr)
#define log_error_to_if(impl, cond, expr)     _cxxtools_log_if(impl, LOG_ERROR, "ERROR", cond, expr)
#define log_warn_to_if(impl, cond, expr)      _cxxtools_log_if(impl, LOG_WARN, "WARN", cond, expr)
#define log_info_to_if(impl, cond, expr)      _cxxtools_log_if(impl, LOG_INFO, "INFO", cond, expr)
#define log_debug_to_if(impl, cond, expr)     _cxxtools_log_if(impl, LOG_DEBUG, "DEBUG", cond, expr)
#define log_fine_to_if(impl, cond, expr)      _cxxtools_log_if(impl, LOG_FINE, "FINE", cond, expr)
#define log_finer_to_if(impl, cond, expr)     _cxxtools_log_if(impl, LOG_FINER, "FINER", cond, expr)
#define log_finest_to_if(impl, cond, expr)    _cxxtools_log_if(impl, LOG_FINEST, "FINEST", cond, expr)

#define log_trace_to(impl, expr)     \
  ::cxxtools::LogTracer _cxxtools_tracer;  \
  do { \
    ::cxxtools::Logger* _cxxtools_logger = getLogger ## impl(); \
    if (_cxxtools_logger != 0 && _cxxtools_logger->isEnabled(::cxxtools::Logger::LOG_TRACE)) \
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
      Logger(const Logger&);
      Logger& operator=(const Logger&);

    public:
      enum log_flag_type {
        LOG_FATAL  = 0x01,
        LOG_ERROR  = 0x02,
        LOG_WARN   = 0x04,
        LOG_INFO   = 0x08,
        LOG_DEBUG  = 0x10,
        LOG_FINE   = 0x10,
        LOG_FINER  = 0x20,
        LOG_FINEST = 0x40,
        LOG_TRACE  = 0x80
      };

      enum log_level_type {
        LOG_LEVEL_FATAL  = (LOG_FATAL << 1) - 1,
        LOG_LEVEL_ERROR  = (LOG_ERROR << 1) - 1,
        LOG_LEVEL_WARN   = (LOG_WARN << 1) - 1,
        LOG_LEVEL_INFO   = (LOG_INFO << 1) - 1,
        LOG_LEVEL_DEBUG  = (LOG_DEBUG << 1) - 1,
        LOG_LEVEL_FINE   = (LOG_FINE << 1) - 1,
        LOG_LEVEL_FINER  = (LOG_FINER << 1) - 1,
        LOG_LEVEL_FINEST = (LOG_FINEST << 1) - 1,
        LOG_LEVEL_TRACE  = LOG_LEVEL_DEBUG | LOG_TRACE
      };

    private:
      std::string category;
      int flags;

    public:
      /// Initalizes a new logger.
      Logger(const std::string& c, int f)
        : category(c), flags(f)
        { }

      const std::string& getCategory() const
        { return category; }
      bool isEnabled(log_flag_type l) const
        { return (flags & l) != 0; }
      log_level_type getLogLevel() const
        { return static_cast<log_level_type>(flags); }
      int getLogFlags() const
        { return flags; }
      void setLogFlags(int f)
        { flags = f; }
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
      /**
       Converts a complex log level string to log_level_type.
       Throws exception when level string could not be interpreted

       accepted log levels:

        * everything below level:
            FATAL
            ERROR
            WARN
            INFO
            DEBUG
            FINE
            FINER
            FINEST
            TRACE

        * everything below level with trace enabled:
            TFATAL
            ...
            TFINE

        * specific log levels:
            TRACE|ERROR|WARN

        * just one log level:
            |WARN

       */
      static int strToLogFlags(const std::string& level);

      typedef Logger::log_level_type log_level_type;

      LogConfiguration();
      LogConfiguration(const LogConfiguration&);
      LogConfiguration& operator=(const LogConfiguration&);

      ~LogConfiguration();

      Impl* impl()             { return _impl; }
      const Impl* impl() const { return _impl; }

      int rootFlags() const;
      int logFlags(const std::string& category) const;

      // setter
      void setRootFlags(int flags);
      void setRootLevel(log_level_type level)
        { setRootFlags(static_cast<int>(level)); }
      /// Sets root level using string notation.
      /// See method strToLogFlags for syntax of string.
      void setRootLevel(const std::string& level)
        { setRootFlags(strToLogFlags(level)); }

      void setLogFlags(const std::string& category, int flags);
      void setLogLevel(const std::string& category, log_level_type level)
        { setLogFlags(category, static_cast<int>(level)); };
      /// Sets specific level using string notation.
      /// See method strToLogFlags for syntax of string.
      /// Empty level removes the specific setting for the category.
      void setLogLevel(const std::string& category, const std::string& level);

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
      static void disable()
      { _enabled = false; }

      int rootFlags() const;
      int logFlags(const std::string& category) const;
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
