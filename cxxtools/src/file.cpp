/***************************************************************************
 *   Copyright (C) 2006-2007 Marc Boris Duerner                            *
 *   Copyright (C) 2006-2007 Tobias Mueller                                *
 *   Copyright (C) 2006-2007 PTV AG                                        *
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
// TODO: autotools
#if defined(_WIN32) || defined(WIN32)
    #include "fileimpl.win32.h"
#else
    #include "fileimpl.posix.h"
#endif

#include "cxxtools/file.h"

namespace cxxtools {

File::File(const std::string& path)
{
    _impl = new FileImpl(path);
}


File::File(const File& file)
{
    _impl = new FileImpl( *file._impl );
}


File::~File()
{
    delete _impl;
}


File& File::operator=(const File& file)
{
    delete _impl;
    _impl = new FileImpl(*file._impl);
    return *this;
}


const std::string& File::path() const
{
    return _impl->path();
}


std::string File::name() const
{
    //TODO: need Environment class to clean this up
#if defined(_WIN32) || defined(WIN32)
    std::string::size_type separatorPos = path().rfind('\\');
#else
    std::string::size_type separatorPos = path().rfind('/');
#endif

    if (separatorPos != std::string::npos)
    {
        return path().substr(separatorPos + 1);
    }

    return path();
}

std::size_t File::size() const
{
    return _impl->size();
}

} // namespace cxxtools
