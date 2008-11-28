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

namespace cxxtools {

void LibraryImpl::open(const std::string& path)
{
    if(_handle)
        return;

    /* RTLD_NOW: since lazy loading is not supported by every target platform
        RTLD_GLOBAL: make the external symbols in the loaded library available for subsequent libraries.
                    see also http://gcc.gnu.org/faq.html#dso
    */
    int flags = RTLD_NOW | RTLD_GLOBAL;

    _handle = ::dlopen(path.c_str(), flags);
    if( !_handle )
    {
        throw OpenLibraryFailed( dlerror(), CXXTOOLS_SOURCEINFO );
    }
}


void LibraryImpl::close()
{
    if(_handle)
        ::dlclose(_handle);
}


void* LibraryImpl::resolve(const char* symbol) const
{
    if(_handle)
    {
        return ::dlsym(_handle, symbol);
    }

    return 0;
}

} // namespace cxxtools