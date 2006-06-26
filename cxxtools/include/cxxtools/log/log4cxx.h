/* cxxtools/log/log4cxx.h
 *
 * cxxtools - general purpose C++-toolbox
 * Copyright (C) 2003, Tommi Maekitalo
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

#ifndef CXXTOOLS_LOG_LOG4CXX_H
#define CXXTOOLS_LOG_LOG4CXX_H

#include <log4cxx/logger.h>
#include <sstream>

#ifndef LOG4CXX_LOCATION
# define LOG4CXX_LOCATION __FILE__, __LINE__
#endif

#define log_fatal_enabled()  getLogger()->isEnabledFor(::log4cxx::Level::FATAL)
#define log_error_enabled()  getLogger()->isEnabledFor(::log4cxx::Level::ERROR)
#define log_warn_enabled()   getLogger()->isEnabledFor(::log4cxx::Level::WARN)
#define log_info_enabled()   getLogger()->isEnabledFor(::log4cxx::Level::INFO)
#define log_debug_enabled()  getLogger()->isEnabledFor(::log4cxx::Level::DEBUG)
#define log_trace_enabled()  getLogger()->isEnabledFor(::log4cxx::Level::DEBUG)

#define log_xxxx(level, expr) \
  do { \
    if (getLogger()->isEnabledFor(::log4cxx::Level::level)) \
    { \
      std::ostringstream _msg_; \
      _msg_ << expr; \
      getLogger()->log( \
        ::log4cxx::Level::level, \
        _msg_.str(), \
        LOG4CXX_LOCATION); \
    } \
  } while (false)

#define log_fatal(expr)  log_xxxx(FATAL, expr)
#define log_error(expr)  log_xxxx(ERROR, expr)
#define log_warn(expr)   log_xxxx(WARN, expr)
#define log_info(expr)   log_xxxx(INFO, expr)
#define log_debug(expr)  log_xxxx(DEBUG, expr)
#define log_trace(expr)  \
  ::cxxtools::log4cxx_tracer tracer ## __LINE__ (getLogger());  \
  if (log_trace_enabled()) \
  { \
    tracer ## __LINE__ .logentry() << expr;  \
    tracer ## __LINE__ .enter();  \
  }

#define log_define(category) \
  static inline log4cxx::LoggerPtr& getLogger()   \
  { \
    static log4cxx::LoggerPtr l = ::log4cxx::Logger::getLogger(category); \
    return l; \
  }

namespace cxxtools
{
  class log4cxx_tracer
  {
      log4cxx::LoggerPtr l;
      std::ostringstream* msg;

      log4cxx::LoggerPtr& getLogger()  { return l; }

    public:
      log4cxx_tracer(::log4cxx::LoggerPtr& l_)
        : l(l_), msg(0)
      { }
      ~log4cxx_tracer()
      {
        if (msg && log_trace_enabled())
          log_debug("EXIT " << msg->str());
      }

      std::ostream& logentry()
      {
        if (!msg)
          msg = new std::ostringstream();
        return *msg;
      }

      void enter()
      {
        if (msg && log_trace_enabled())
          log_debug("ENTER " << msg->str());
      }
  };
}

#endif // CXXTOOLS_LOG_LOG4CXX_H
