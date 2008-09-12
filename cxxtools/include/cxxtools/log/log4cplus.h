/* cxxtools/log/log4cplus.h
 *
 * cxxtools - general purpose C++-toolbox
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
 *
 */

#ifndef CXXTOOLS_LOG_LOG4CPLUS_H
#define CXXTOOLS_LOG_LOG4CPLUS_H

#include <log4cplus/logger.h>

#define log_fatal_enabled()  getLogger().isEnabledFor(::log4cplus::FATAL_LOG_LEVEL)
#define log_error_enabled()  getLogger().isEnabledFor(::log4cplus::ERROR_LOG_LEVEL)
#define log_warn_enabled()   getLogger().isEnabledFor(::log4cplus::WARN_LOG_LEVEL)
#define log_info_enabled()   getLogger().isEnabledFor(::log4cplus::INFO_LOG_LEVEL)
#define log_debug_enabled()  getLogger().isEnabledFor(::log4cplus::DEBUG_LOG_LEVEL)
#define log_trace_enabled()  getLogger().isEnabledFor(::log4cplus::TRACE_LOG_LEVEL)

#define log_xxxx(level, expr)  \
    do { \
      if (getLogger().getRoot().getAllAppenders().size() > 0) \
      { \
        LOG4CPLUS_ ## level (getLogger(), expr) \
      } \
    } while(false)

#define log_fatal(expr)  log_xxxx(FATAL, expr)
#define log_error(expr)  log_xxxx(ERROR, expr)
#define log_warn(expr)   log_xxxx(WARN, expr)
#define log_info(expr)   log_xxxx(INFO, expr)
#define log_debug(expr)  log_xxxx(DEBUG, expr)
#define log_trace(expr) \
  ::cxxtools::log4cplus_tracer tracer ## __LINE__ (getLogger());  \
  if (log_trace_enabled()) \
  { \
    tracer ## __LINE__ .logentry() << expr;  \
    tracer ## __LINE__ .enter();  \
  }

#define log_define(category)   \
  static inline log4cplus::Logger& getLogger()   \
  {  \
    static log4cplus::Logger tntlogger = log4cplus::Logger::getInstance(category);  \
    return tntlogger;  \
  }

namespace cxxtools
{
  class log4cplus_tracer
  {
      log4cplus::Logger& l;
      std::ostringstream* msg;

      log4cplus::Logger& getLogger()  { return l; }

    public:
      log4cplus_tracer(::log4cplus::Logger& l_)
        : l(l_), msg(0)
      { }
      ~log4cplus_tracer()
      {
        if (msg && log_trace_enabled())
          LOG4CPLUS_TRACE(l, "EXIT " << msg->str());
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
          LOG4CPLUS_TRACE(l, "ENTER " << msg->str());
      }
  };
}

#endif // CXXTOOLS_LOG_LOG4CPLUS_H
