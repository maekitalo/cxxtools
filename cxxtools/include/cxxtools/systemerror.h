/***************************************************************************
 *   Copyright (C) 2004-2007 Marc Boris Duerner                            *
 *   Copyright (C) 2005-2007 Aloysius Indrayanto                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   As a special exception, you may use this file as part of a free       *
 *   software library without restriction. Specifically, if other files    *
 *   instantiate templates or use macros or inline functions from this     *
 *   file, or you compile this file and link it with other files to        *
 *   produce an executable, this file does not by itself cause the         *
 *   resulting executable to be covered by the GNU General Public          *
 *   License. This exception does not however invalidate any other         *
 *   reasons why the executable file might be covered by the GNU Library   *
 *   General Public License.                                               *
 *                                                                         *
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
#ifndef CXXTOOLS_SYSTEM_ERROR_H
#define CXXTOOLS_SYSTEM_ERROR_H

#include <cxxtools/sourceinfo.h>
#include <stdexcept>

namespace cxxtools {

/** @brief Exception class indicating a system error.
 */
class SystemError : public std::runtime_error
{
    public:
        SystemError(int err, const char* fn);

        SystemError(const char* fn);

        SystemError(const std::string& what, const SourceInfo& si);

        ~SystemError() throw();

        //int getErrno() const
        //{ return m_errno; }

    private:
        int m_errno;
};

/** @brief Thrown, when a shared library could not be loaded
*/
class OpenLibraryFailed : public SystemError
{
    public:
        //! @brief Contructs from a message string and source info
        OpenLibraryFailed(const std::string& msg, const cxxtools::SourceInfo& si);

        //! @brief Destructor
        ~OpenLibraryFailed() throw()
		{}
};

/** @brief Thrown, when a symbol is not found in a library
*/
class SymbolNotFound : public SystemError
{
    std::string _symbol;

    public:
        SymbolNotFound(const std::string& sym, const cxxtools::SourceInfo& si);

        //! @brief Destructor
        ~SymbolNotFound() throw()
        {}

        //! @brief Returns the symbol, which was not found
        const std::string& symbol() const
        { return _symbol; }
};

} // namespace cxxtools

#endif
