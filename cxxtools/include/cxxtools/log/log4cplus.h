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

#define log_fatal(expr)  do { LOG4CPLUS_FATAL(getLogger(), expr) } while(false)
#define log_error(expr)  do { LOG4CPLUS_ERROR(getLogger(), expr) } while(false)
#define log_warn(expr)   do { LOG4CPLUS_WARN(getLogger(), expr) } while(false)
#define log_info(expr)   do { LOG4CPLUS_INFO(getLogger(), expr) } while(false)
#define log_debug(expr)  do { LOG4CPLUS_DEBUG(getLogger(), expr) } while(false)

#define log_define(category)   \
  static inline log4cplus::Logger& getLogger()   \
  {  \
    static log4cplus::Logger tntlogger = log4cplus::Logger::getInstance(category);  \
    return tntlogger;  \
  }

#endif // CXXTOOLS_LOG_LOG4CPLUS_H
