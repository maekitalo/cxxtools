/*
 * Copyright (C) 2004-2007 Marc Boris Duerner
 * Copyright (C)      2006 Aloysius Indrayanto
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
#ifndef cxxtools_SourceInfo_h
#define cxxtools_SourceInfo_h

#include <string>

// GNU C++ compiler
#ifdef __GNUC__
    #define CXXTOOLS_FUNCTION __PRETTY_FUNCTION__
// Borland C++
#elif defined(__BORLANDC__)
    #define CXXTOOLS_FUNCTION __FUNC__
// Microsoft C++ compiler
#elif defined(_MSC_VER)
    // .NET 2003 support's demangled function names
    #if _MSC_VER >= 1300
        #define CXXTOOLS_FUNCTION __FUNCDNAME__
    #else
        #define CXXTOOLS_FUNCTION __FUNCTION__
    #endif
// otherwise use standard macro
#else
    #define CXXTOOLS_FUNCTION "unknown symbol"
#endif

#define CXXTOOLS_STRINGIFY(x) #x
#define CXXTOOLS_TOSTRING(x) CXXTOOLS_STRINGIFY(x)

#define CXXTOOLS_SOURCEINFO_STR __FILE__ ":" CXXTOOLS_TOSTRING(__LINE__)

/** @brief Builds a message including source information
*/
#define CXXTOOLS_ERROR_MSG(msg) __FILE__ ":" CXXTOOLS_TOSTRING(__LINE__) ": " #msg

/** @brief Construct a SourceInfo object
*/
#define CXXTOOLS_SOURCEINFO cxxtools::SourceInfo(__FILE__, CXXTOOLS_TOSTRING(__LINE__), CXXTOOLS_FUNCTION)

namespace cxxtools {

/** @brief Source code info class
    @ingroup Pt

    This class is used to store information about a location in the source 
    code. The CXXTOOLS_SOURCEINFO macro can be used to construct a Pt::SourceInfo
    object conveniently.

    @code
        Pt::SourceInfo si(PT_SOURCEINFO);

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
        /** @brief Constructor

            Do not use the constructor directly, but the PT_SOURCEINFO
            macro to take advantage of compiler specific macros to
            indicate the source file name, position and function name.
        */
        inline SourceInfo(const char* file, const char* line, const char* func)
        : _file(file), _line(line), _func(func)
        { }

        /**  @brief Returns the filename
        */
        inline const char* file() const
        { return _file; }

        /** @brief Returns the line number
        */
        inline const char* line() const
        { return _line; }

        /** @brief Returns the function signature
        */
        inline const char* func() const
        { return _func; }

    private:
        const char* _file;
        const char* _line;
        const char* _func;
};


inline std::string operator+(const std::string& what, const SourceInfo& info)
{
    return std::string( info.file() ) + ':' + info.line() + ": " += what;
}

inline std::string operator+(const char* what, const SourceInfo& info)
{
    return std::string( info.file() ) + ':' + info.line() + ": " += what;
}

inline std::string operator+( const SourceInfo& info, const std::string& what)
{
    return std::string( info.file() ) + ':' + info.line() + ": " += what;
}

inline std::string operator+(const SourceInfo& info, const char* what)
{
    return std::string( info.file() ) + ':' + info.line() + ": " += what;
}

} // namespace cxxtools

#endif
