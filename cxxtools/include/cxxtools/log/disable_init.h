/* cxxtools/log/disable_init.h
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

#ifndef CXXTOOLS_LOG_DISABLE_INIT_H
#define CXXTOOLS_LOG_DISABLE_INIT_H

#include <string>

#define log_init_fatal()  do { } while(false)
#define log_init_error()  do { } while(false)
#define log_init_warn()   do { } while(false)
#define log_init_info()   do { } while(false)
#define log_init_debug()  do { } while(false)

inline void log_init(const std::string& configFileName)
{ }

inline void log_init(const char* filename)
{ }

inline void log_init()
{ }

#endif // CXXTOOLS_LOG_DISABLE_INIT_H
