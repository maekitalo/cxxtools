/* cxxtools/log/log4cxx.h
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

#ifndef CXXTOOLS_LOG_LOG4CXX_H
#define CXXTOOLS_LOG_LOG4CXX_H

#include <log4cxx/logger.h>
#include <sstream>

#define log_xxxx(level, expr) \
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

#define log_fatal(expr)  log_xxxx(FATAL, expr)
#define log_error(expr)  log_xxxx(ERROR, expr)
#define log_warn(expr)   log_xxxx(WARN, expr)
#define log_info(expr)   log_xxxx(INFO, expr)
#define log_debug(expr)  log_xxxx(DEBUG, expr)

#define log_define(category) \
  static inline log4cxx::LoggerPtr& getLogger()   \
  { \
    static log4cxx::LoggerPtr l = ::log4cxx::Logger::getLogger(category); \
    return l; \
  }

#endif // CXXTOOLS_LOG_LOG4CXX_H
