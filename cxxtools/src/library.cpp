/***************************************************************************
 *   Copyright (C) 2006-2008 Marc Boris Duerner                            *
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
#include "libraryimpl.h"
#include "cxxtools/library.h"
#include "cxxtools/fileinfo.h"
#include "cxxtools/file.h"
#include "cxxtools/directory.h"
#include <string>
#include <memory>

namespace cxxtools {

Library::Library()
: _impl(0)
{
    _impl = new LibraryImpl();
}


Library::Library(const std::string& path)
: _impl(0)
{
    _path = find(path);
    _impl = new LibraryImpl(_path);
}


Library::Library(const Library& other)
{
    _path = other._path;
    _impl = other._impl;
    _impl->ref();
}


Library& Library::operator=(const Library& other)
{
    if(_impl == other._impl)
        return *this;

    _path = other._path;

    other._impl->ref();

    if( ! _impl->unref() )
        delete _impl;

    _impl = other._impl;

    return *this;
}


Library::~Library()
{
    if ( ! _impl->unref() )
        delete _impl;
}


void Library::detach()
{
    if ( _impl->refs() == 1 )
        return;

    _path.clear();

    LibraryImpl* x = _impl;
    _impl = new LibraryImpl();

    if( ! x->unref() )
        delete x;
}

Library& Library::open(const std::string& path)
{
    this->detach();

    _path = find(path);
    _impl->open(path);
    return *this;
}


void Library::close()
{
    this->detach();

    _impl->close();
}


void* Library::resolve(const char* symbol) const
{
  return _impl->resolve(symbol);
}


Symbol Library::getSymbol(const char* symbol) const
{
    void* sym = this->resolve(symbol);
    if (sym == 0)
    {
        throw SymbolNotFound(symbol, CXXTOOLS_SOURCEINFO);
    }

    return Symbol(*this, sym);
}

Library::operator const void*() const
{
    return _impl->failed() ? 0 : this;
}


bool Library::operator!() const
{
    return _impl->failed() ? true : false;
}


const std::string& Library::path() const
{
    return _path;
}


std::string Library::find(const std::string& path_)
{
    std::string path = path_;

    if( FileInfo::exists( path ) )
        return path;

    std::string::size_type idx = path.find( Directory::sep() );

    if( ++idx == path.length() )
    {
	    throw FileNotFound(path , CXXTOOLS_SOURCEINFO);
    }

    path += suffix();
    if( FileInfo::exists( path ) )
	{
        return path;
	}

    if(idx == std::string::npos)
    {
        idx = 0;
    }
    path.insert( idx, prefix() );

    if( ! FileInfo::exists( path ) )
    {
        throw FileNotFound(path , CXXTOOLS_SOURCEINFO);
    }

    return path;
}


std::string Library::suffix()
{
    return LibraryImpl::suffix();
}


std::string Library::prefix()
{
    return LibraryImpl::prefix();
}

} // namespace cxxtools
