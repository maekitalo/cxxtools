/* cxxtools/log/cxxtools_init.h
   Copyright (C) 2004 Tommi Maekitalo

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

#ifndef CXXTOOLS_LOG_CXXTOOLS_INIT_H
#define CXXTOOLS_LOG_CXXTOOLS_INIT_H

#include <fstream>
#include <cxxtools/log/cxxtools.h>

#define log_init_fatal()   log_init(cxxtools::logger::LOG_LEVEL_FATAL)
#define log_init_error()   log_init(cxxtools::logger::LOG_LEVEL_ERROR)
#define log_init_warn()    log_init(cxxtools::logger::LOG_LEVEL_WARN)
#define log_init_info()    log_init(cxxtools::logger::LOG_LEVEL_INFO)
#define log_init_debug()   log_init(cxxtools::logger::LOG_LEVEL_DEBUG)
#define log_init_trace()   log_init(cxxtools::logger::LOG_LEVEL_TRACE)
#define log_init           log_init_cxxtools

inline void log_init_cxxtools(cxxtools::logger::log_level_type level)
{
  cxxtools::logger::setRootLevel(level);
}

void log_init_cxxtools(const std::string& propertyfilename);

void log_init_cxxtools();

#endif // CXXTOOLS_LOG_CXXTOOLS_INIT_H
