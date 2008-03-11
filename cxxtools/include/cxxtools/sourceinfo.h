/***************************************************************************
 *   Copyright (C) 2004-2007 Marc Boris Duerner                            *
 *   Copyright (C)      2006 Aloysius Indrayanto                           *
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
#ifndef cxxtools_SourceInfo_h
#define cxxtools_SourceInfo_h

#include <cxxtools/api.h>
#include <string>

// GNU C++ compiler
#ifdef __GNUC__
    #define CXXTOOLS_PRETTY_FUNCTION __PRETTY_FUNCTION__
// Borland C++
#elif defined(__BORLANDC__)
    #define CXXTOOLS_PRETTY_FUNCTION __FUNC__
// Microsoft C++ compiler
#elif defined(_MSC_VER)
    // .NET 2003 support's demangled function names
    #if _MSC_VER >= 1300
        #define CXXTOOLS_PRETTY_FUNCTION __FUNCDNAME__
    #else
        #define CXXTOOLS_PRETTY_FUNCTION __FUNCTION__
    #endif
// otherwise use standard macro
#else
    #define CXXTOOLS_PRETTY_FUNCTION "unknown symbol"
#endif

#define CXXTOOLS_SOURCEINFO_STRINGIFY(x) #x
#define CXXTOOLS_SOURCEINFO_TOSTRING(x) CXXTOOLS_SOURCEINFO_STRINGIFY(x)

/** @brief Builds a message including source information
*/
#define CXXTOOLS_SOURCEINFO_MSG(msg) __FILE__ ":" CXXTOOLS_SOURCEINFO_TOSTRING(__LINE__) ": " #msg

/** @brief Construct a SourceInfo object
*/
#define CXXTOOLS_SOURCEINFO cxxtools::SourceInfo(__FILE__, __LINE__, CXXTOOLS_PRETTY_FUNCTION, \
                                     __FILE__ ":" CXXTOOLS_SOURCEINFO_TOSTRING(__LINE__) )

namespace cxxtools {

/** @brief Source code info class

    This class is used to store information about a location in the source 
    code. The CXXTOOLS_SOURCEINFO macro can be used to construct a SourceInfo
    object conveniently.

    @code
        SourceInfo si(CXXTOOLS_SOURCEINFO);

        // print file, line and function
        std::cout << si.file() << std::endl;
        std::cout << si.line() << std::endl;
        std::cout << si.func() << std::endl;

        // print combined string
        std::cout << si.where() << std::endl;
    @endcode
*/
class SourceInfo {
    public:
        /** @brief Copy constructor
        */
        inline SourceInfo(const SourceInfo& si) throw()
        : _file(si._file), _line(si._line), _func(si._func), _msg(si._msg)
        { }

        /** @brief Constructor

            Do not use the constructor directly, but the CXXTOOLS_SOURCEINFO
            macro to take advantage of compiler specific macros to
            indicate the source file name, position and function name.
        */
        inline SourceInfo(const char* file, unsigned int line, const char* func, const char* msg) throw()
        : _file(file), _line(line), _func(func), _msg(msg)
        { }

        /**  @brief Returns the filename
        */
        inline const char* file() const throw()
        { return _file; }

        /** @brief Returns the line number
        */
        inline unsigned int line() const throw()
        { return _line; }

        /** @brief Returns a string describing the location
        */
        inline const char* where() const
        { return _msg; }

        /** @brief Returns the function signature
        */
        inline const char* func() const throw()
        { return _func; }

        /** @brief Assignment operator
        */
        SourceInfo& operator=(const SourceInfo& si) throw()
        {
            _file = si._file;
            _line = si._line;
            _func = si._func;
            _msg = si._msg;
            return *this;
        }

    private:
        const char*  _file;
        unsigned int _line;
        const char*  _func;
        const char* _msg;
};


inline std::string operator+(const std::string& what, const SourceInfo& info)
{
    return std::string( info.where() ) + ": " + what;
}

} // namespace cxxtools

#endif
