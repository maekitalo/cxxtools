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
#include "cxxtools/systemerror.h"
#include "cxxtools/log.h"
#include <errno.h>
#include "error.h"

log_define("cxxtools.systemerror")

namespace cxxtools
{

SystemError::SystemError(int err, const char* fn)
: std::runtime_error( getErrnoString(err, fn) )
, m_errno(err)
{
  //log_debug("system error; " << what());
}


SystemError::SystemError(const char* fn)
: std::runtime_error( getErrnoString(fn) )
, m_errno(errno)
{
  //log_debug("system error; " << what());
}


SystemError::SystemError(const char* fn, const std::string& what)
: std::runtime_error(fn && fn[0] ? (std::string("error in function ") + fn + ": " + what) : what),
  m_errno(0)
{
  //log_debug("system error; " << std::exception::what());
}


SystemError::~SystemError() throw()
{ }

OpenLibraryFailed::OpenLibraryFailed(const std::string& msg)
: SystemError("", msg)
{
  log_debug("open library failed; " << what());
}

SymbolNotFound::SymbolNotFound(const std::string& sym)
: SystemError("", "symbol not found: " + sym)
, _symbol(sym)
{
  log_debug("symbol " << sym << " not found; " << what());
}

} // namespace cxxtools
