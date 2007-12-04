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
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdexcept>

namespace cxxtools {

class FileImpl
{
    public:
        FileImpl(const std::string& path)
        : _path(path)
        {
        }

        ~FileImpl()
        {}

        std::size_t size() const
        {
            struct stat buff;

            if( 0 != stat(_path.c_str(), &buff) )
                throw std::runtime_error("stat() failed");

            return buff.st_size;
        }

        const std::string& path()
        { return _path; }

    private:
        std::string _path;
};

} // namespace cxxtools
