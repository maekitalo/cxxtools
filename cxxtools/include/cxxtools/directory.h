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
#ifndef CXXTOOLS_DIRECTORY_H
#define CXXTOOLS_DIRECTORY_H

#include <cxxtools/api.h>
#include <cxxtools/filesystemnode.h>
#include <string>
#include <cstddef>

namespace cxxtools {

class CXXTOOLS_API Directory : public FileSystemNode 
{
    public:
        Directory(const std::string& path = "");

        ~Directory();

        virtual const std::string& path() const;

        virtual std::string name() const;

        virtual std::size_t size() const;

        virtual Type type() const
        {
            return FileSystemNode::Directory;
        }

    private:
        std::string _path;
};

}

#endif
