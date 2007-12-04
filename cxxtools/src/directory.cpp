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
#include "cxxtools/directory.h"

namespace cxxtools {

Directory::~Directory()
{
}


const std::string& Directory::path() const
{
    return _path;
}


std::string Directory::name() const
{
    //TODO: need Environment class to clean this up
#if defined(_WIN32) || defined(WIN32)
    std::string::size_type separatorPos = _path.rfind('\\');
#else
    std::string::size_type separatorPos = _path.rfind('/');
#endif

    if (separatorPos != std::string::npos)
    {
        return _path.substr(separatorPos + 1);
    }
    else
    {
        return _path;
    }
}


std::size_t Directory::size() const
{
    return 0;
}

} // namespace cxxtools

