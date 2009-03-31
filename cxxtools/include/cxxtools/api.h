/*
 * Copyright (C) 2006-2008 by Marc Boris Duerner
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
 */
#ifndef cxxtools_Api_h
#define cxxtools_Api_h

#include <cxxtools/config.h>

#if defined(WIN32) || defined(_WIN32)
    // suppress min/max macros from win32 headers
    #ifndef NOMINMAX
    #define NOMINMAX
    #endif

    // Use of features specific Windows versions
    #ifndef WINVER
    #define WINVER 0x0501
    #endif

    #ifndef _WIN32_WINNT
    #define _WIN32_WINNT 0x0501
    #endif

    #ifndef _WIN32_WINDOWS
    #define _WIN32_WINDOWS 0x0410
    #endif
#endif

#if defined(WIN32) || defined(_WIN32)
    #define CXXTOOLS_EXPORT __declspec(dllexport)
    #define CXXTOOLS_IMPORT __declspec(dllimport)
#else
    #define CXXTOOLS_EXPORT
    #define CXXTOOLS_IMPORT
#endif

#if defined(CXXTOOLS_API_EXPORT)
    #define CXXTOOLS_API CXXTOOLS_EXPORT
#else
    #define CXXTOOLS_API CXXTOOLS_IMPORT
# endif

#endif
