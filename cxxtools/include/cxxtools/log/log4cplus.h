/* cxxtools/log/log4cplus.h
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

#ifndef CXXTOOLS_LOG_LOG4CPLUS_H
#define CXXTOOLS_LOG_LOG4CPLUS_H

#include <log4cplus/logger.h>

#define log_fatal_enabled()  getLogger().isEnabledFor(::log4cplus::FATAL_LOG_LEVEL)
#define log_error_enabled()  getLogger().isEnabledFor(::log4cplus::ERROR_LOG_LEVEL)
#define log_warn_enabled()   getLogger().isEnabledFor(::log4cplus::WARN_LOG_LEVEL)
#define log_info_enabled()   getLogger().isEnabledFor(::log4cplus::INFO_LOG_LEVEL)
#define log_debug_enabled()  getLogger().isEnabledFor(::log4cplus::DEBUG_LOG_LEVEL)
#define log_trace_enabled()  getLogger().isEnabledFor(::log4cplus::TRACE_LOG_LEVEL)

#define log_fatal(expr)  do { LOG4CPLUS_FATAL(getLogger(), expr) } while(false)
#define log_error(expr)  do { LOG4CPLUS_ERROR(getLogger(), expr) } while(false)
#define log_warn(expr)   do { LOG4CPLUS_WARN(getLogger(), expr) } while(false)
#define log_info(expr)   do { LOG4CPLUS_INFO(getLogger(), expr) } while(false)
#define log_debug(expr)  do { LOG4CPLUS_DEBUG(getLogger(), expr) } while(false)
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
