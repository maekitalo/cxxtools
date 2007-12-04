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
#ifndef cxxtools_File_h
#define cxxtools_File_h

#include <cxxtools/api.h>
#include <cxxtools/filesystemnode.h>

namespace cxxtools {

class CXXTOOLS_API File : public FileSystemNode
{
    private:
        class FileImpl* _impl;

    public:
        File(const std::string& path = "");

        File(const File& file);

        ~File();

        File& operator=(const File& file);

        virtual const std::string& path() const;

        virtual std::size_t size() const;

        virtual std::string name() const;

        virtual Type type() const
        {
            return FileSystemNode::File; 
        }
};

} // namespace cxxtools

#endif
