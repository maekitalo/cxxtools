/* cxxtools/log/disable.h
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

#ifndef CXXTOOLS_LOG_DISABLE_H
#define CXXTOOLS_LOG_DISABLE_H

#define log_fatal_enable()        false
#define log_error_enable()        false
#define log_warn_enable()         false
#define log_info_enable()         false
#define log_debug_enable()        false
#define log_trace_enable()        false

#define log_fatal(expr)        do { } while (false)
#define log_error(expr)        do { } while (false)
#define log_warn(expr)         do { } while (false)
#define log_info(expr)         do { } while (false)
#define log_debug(expr)        do { } while (false)
#define log_trace(expr)        do { } while (false)

#define log_define(category)

#endif // CXXTOOLS_LOG_DISABLE_H
