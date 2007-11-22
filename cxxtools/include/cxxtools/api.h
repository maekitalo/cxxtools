/***************************************************************************
 *   Copyright (C) 2004-2007 Marc Boris Duerner                            *
 *   Copyright (C) 2006 by Aloysius Indrayanto                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef cxxtools_Api_h
#define cxxtools_Api_h

#if defined(_MSC_VER)
	//http://support.microsoft.com/support/kb/articles/Q134/9/80.asp
	//http://support.microsoft.com/support/kb/articles/Q168/9/58.ASP
	//http://support.microsoft.com/support/kb/articles/Q172/3/96.ASP

	// deprectated stdc++ functions
	#pragma warning( disable : 4996 )

	// dll-linkage
	#pragma warning( disable : 4251 )

	//non dll-interface base class
	#pragma warning( disable : 4275 )

	// exception sepcification ignored
	#pragma warning( disable : 4290 )
#endif

#if defined (__INTEL_COMPILER)
    // field of class type without a DLL interface used in a class with a DLL interface
    #pragma warning( disable : 1744 )

    // base class dllexport/dllimport specification differs from that of the derived class
    #pragma warning( disable : 1738 )
#endif

#if defined(WIN32) || defined(_WIN32)
    // suppress min/max macros from win32 headers
    #ifndef NOMINMAX
    #define NOMINMAX
    #endif
#endif

#if defined(_MSC_VER) || defined(WIN32) || defined(_WIN32)
    #define CXXTOOLS_EXPORT __declspec(dllexport)
    #define CXXTOOLS_IMPORT __declspec(dllimport)    
#elif __GNUC__ >= 4
    #define CXXTOOLS_EXPORT __attribute__((visibility("default")))
    #define CXXTOOLS_IMPORT
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
