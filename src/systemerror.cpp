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
#include <cxxtools/systemerror.h>
#include <cxxtools/log.h>

#include <mutex>
#include <condition_variable>

#include <openssl/err.h>
#include <openssl/ssl.h>

#include <errno.h>
#include "error.h"

log_define("cxxtools.systemerror")

namespace cxxtools
{
SystemError::SystemError(const std::string& msg)
: std::runtime_error(msg),
  m_errno(0)
{
  log_finer("system error: " << what());
}


SystemError::SystemError(int err, const char* fn)
: std::runtime_error(getErrnoString(err, fn)),
  m_errno(err)
{
  log_finer("system error: " << what());
}


SystemError::SystemError(const char* fn)
: std::runtime_error(getErrnoString(fn)),
  m_errno(errno)
{
  log_finer("system error: " << what());
}


SystemError::SystemError(const char* fn, const std::string& what)
: std::runtime_error(fn && fn[0] ? (std::string("function ") + fn + " failed: " + what) : what),
  m_errno(0)
{
  log_finer("system error: " << std::exception::what());
}


SystemError::~SystemError() throw()
{ }

void throwSystemError(const char* fn)
{
    throw SystemError(fn);
}

void throwSystemError(int errnum, const char* fn)
{
    throw SystemError(errnum, fn);
}

OpenLibraryFailed::OpenLibraryFailed(const std::string& msg)
: SystemError(msg)
{
  log_finer("open library failed: " << what());
}

SymbolNotFound::SymbolNotFound(const std::string& sym)
: SystemError(0, "symbol not found: " + sym),
  _symbol(sym)
{
  log_finer("symbol " << sym << " not found; " << what());
}

void SslError::checkSslError()
{
    static std::mutex mutex;
    static bool errorStringsLoaded = false;

    unsigned long code = ERR_get_error();
    if (code != 0)
    {
        if (!errorStringsLoaded)
        {
            std::lock_guard<std::mutex> lock(mutex);
            if (!errorStringsLoaded)
            {
                log_debug("SSL_load_error_strings");
                SSL_load_error_strings();
                errorStringsLoaded = true;
            }
        }

        char buffer[120];
        if (ERR_error_string(code, buffer))
        {
            log_debug("SSL-Error " << code << ": \"" << buffer << '"');
            ignorePendingSslErrors();
            throw SslError(buffer, code);
        }
        else
        {
            log_debug("unknown SSL-Error " << code);
            ignorePendingSslErrors();
            throw SslError("unknown SSL-Error", code);
        }
    }
}

void SslError::ignorePendingSslErrors()
{
    char buffer[120];
    while (true)
    {
        auto code = ERR_get_error();
        if (code == 0)
            return;

        if (ERR_error_string(code, buffer))
            log_warn("ssl error " << code << " detected: \"" << buffer << "\" - ignored");
        else
            log_warn("unknown ssl error " << code << " detected - ignored");
    }
}

} // namespace cxxtools
