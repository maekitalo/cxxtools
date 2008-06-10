/***************************************************************************
 *   Copyright (C) 2005-2008 by Marc Boris Duerner                         *
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
#include "fileimpl.h"
#include "cxxtools/systemerror.h"
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

namespace cxxtools {

FileImpl::FileImpl()
{
}


FileImpl::~FileImpl()
{
}


std::size_t FileImpl::size(const std::string& path)
{
    struct stat buff;

    if( 0 != stat(path.c_str(), &buff) )
        throw SystemError("Could not stat file", CXXTOOLS_SOURCEINFO);

    return buff.st_size;
}


void FileImpl::resize(const std::string& path, std::size_t newSize)
{
    int ret = 0;
    do {
        ret = truncate(path.c_str(), newSize);
    } while ( ret == EINTR );

    if(ret != 0)
        throw SystemError("Could not truncate file", CXXTOOLS_SOURCEINFO);
}


void FileImpl::remove(const std::string& path)
{
    if(0 != ::remove(path.c_str()))
        throw SystemError("Could not remove file", CXXTOOLS_SOURCEINFO);
}


void FileImpl::move(const std::string& path, const std::string& to)
{
    if (0 != ::rename(path.c_str(), to.c_str()))
        throw SystemError("Could not move file " + path + " to " + to, CXXTOOLS_SOURCEINFO);
}


void FileImpl::create(const std::string& path)
{
    FILE* f = fopen(path.c_str(), "w");
    if (!f)
        throw SystemError("Could not create file " + path, CXXTOOLS_SOURCEINFO);

    fclose(f);
}

}
