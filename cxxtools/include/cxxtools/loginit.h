/* cxxtools/loginit.h
 *
 * cxxtools - general purpose C++-toolbox
 * Copyright (C) 2004 Tommi Maekitalo
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

#ifndef CXXTOOLS_LOGINIT_H
#define CXXTOOLS_LOGINIT_H

#include <cxxtools/config.h>

#if defined(CXXTOOLS_LOGGING_CXXTOOLS)
#  include <cxxtools/log/cxxtools_init.h>

#elif defined(CXXTOOLS_LOGGING_LOG4CXX)
#  include <cxxtools/log/log4cxx_init.h>

#elif defined(CXXTOOLS_LOGGING_LOG4CPLUS)
#  include <cxxtools/log/log4cplus_init.h>

#else
#  include <cxxtools/log/disable_init.h>

#endif

#endif //  CXXTOOLS_LOGINIT_H
