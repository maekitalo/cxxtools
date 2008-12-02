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
#include "cxxtools/systemerror.h"
#include <errno.h>
#include <string.h>
#include <sstream>

namespace
{
    std::string getErrnoString(int err, const char* fn)
    {
        if (err != 0)
        {
            std::ostringstream msg;
            msg << fn << ": errno " << err << ": " << strerror(err);
            return msg.str();
        }
        else
            return fn;
    }
}

namespace cxxtools {

void throwSysErrorIf(bool flag, const char* fn)
{
    if(flag)
        throw SystemError(fn);
}


void throwSysErrorIf(bool flag, int err, const char* fn)
{
    if(flag)
        throw SystemError(err, fn);
}


SystemError::SystemError(int err, const char* fn)
: std::runtime_error( getErrnoString(err, fn) )
, m_errno(err)
{ }


SystemError::SystemError(const char* fn)
: std::runtime_error( getErrnoString(errno, fn) )
, m_errno(errno)
{}


SystemError::SystemError(const std::string& what, const SourceInfo& si)
: std::runtime_error(what + si)
, m_errno(0)
{ }


SystemError::~SystemError() throw()
{ }

OpenLibraryFailed::OpenLibraryFailed(const std::string& msg, const cxxtools::SourceInfo& si)
: SystemError(msg, si)
{ }

SymbolNotFound::SymbolNotFound(const std::string& sym, const cxxtools::SourceInfo& si)
: SystemError("symbol not found: " + sym, si)
, _symbol(sym)
{ }

} // namespace cxxtools
