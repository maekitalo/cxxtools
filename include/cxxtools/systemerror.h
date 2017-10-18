/*
 * Copyright (C) 2004-2007 Marc Boris Duerner
 * Copyright (C) 2005-2007 Aloysius Indrayanto
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
#ifndef CXXTOOLS_SYSTEM_ERROR_H
#define CXXTOOLS_SYSTEM_ERROR_H

#include <stdexcept>
#include <string>

namespace cxxtools
{

/** @brief Exception class indicating a system error.
 */
class SystemError : public std::runtime_error
{
    protected:
        explicit SystemError(const std::string& msg);

    public:
        SystemError(int err, const char* fn);

        explicit SystemError(const char* fn);

        SystemError(const char* fn, const std::string& msg);

        ~SystemError() throw();

        int getErrno() const
        { return m_errno; }

    private:
        int m_errno;
};

void throwSystemError(const char* fn);

void throwSystemError(int errnum, const char* fn);

/** @brief Thrown, when a shared library could not be loaded
*/
class OpenLibraryFailed : public SystemError
{
        std::string _library;

    public:
        //! @brief Constructs from a message string
        explicit OpenLibraryFailed(const std::string& library);

        //! @brief Destructor
        ~OpenLibraryFailed() throw()
        {}

        const std::string& library() const
        { return _library; }
};

/** @brief Thrown, when a symbol is not found in a library
*/
class SymbolNotFound : public SystemError
{
        std::string _symbol;

    public:
        explicit SymbolNotFound(const std::string& sym);

        //! @brief Destructor
        ~SymbolNotFound() throw()
        {}

        //! @brief Returns the symbol, which was not found
        const std::string& symbol() const
        { return _symbol; }
};

class SslError : public SystemError
{
    public:
        SslError(const std::string& msg, unsigned long sslError)
            : SystemError(msg),
              _sslError(sslError)
            { }

        unsigned long getSslError() const
            { return _sslError; }

        /// \internal
        static void checkSslError();
    private:
        unsigned long _sslError;
};

} // namespace cxxtools

#endif
