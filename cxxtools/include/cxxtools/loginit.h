/* cxxtools/loginit.h
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

#ifndef CXXTOOLS_LOGINIT_H
#define CXXTOOLS_LOGINIT_H

#if  !defined(CXXTOOLS_LOG_LOG4CXX) \
  && !defined(CXXTOOLS_LOG_LOG4CPLUS) \
  && !defined(CXXTOOLS_LOG_LOGBUILTIN) \
  && !defined(CXXTOOLS_LOG_DISABLE)
#  include <cxxtools/config.h>
#endif


#ifdef CXXTOOLS_LOG_LOG4CXX
# include <cxxtools/log/log4cxx_init.h>
#endif


#ifdef CXXTOOLS_LOG_LOG4CPLUS
# include <cxxtools/log/log4cplus_init.h>
#endif


#ifdef CXXTOOLS_LOG_LOGBUILTIN
# include <cxxtools/log/cxxtools_init.h>
#endif


#ifdef CXXTOOLS_LOG_DISABLE
# include <cxxtools/log/disable_init.h>
#endif


#endif //  CXXTOOLS_LOGINIT_H
