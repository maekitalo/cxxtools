/*
 * Copyright (C) 2005-2008 by Marc Boris Duerner
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
#include "fileimpl.h"
#include "error.h"
#include "cxxtools/systemerror.h"
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

#include "config.h"

#if defined(HAVE_SENDFILE) && defined(HAVE_SYS_SENDFILE_H)
#include <sys/sendfile.h>
#endif

namespace cxxtools {

std::size_t FileImpl::size(const std::string& path)
{
    struct stat buff;

    if( 0 != stat(path.c_str(), &buff) )
    {
        throwCurrentErrno("stat", path);
    }

    return buff.st_size;
}


void FileImpl::resize(const std::string& path, std::size_t newSize)
{
    int ret = 0;
    do
    {
        ret = truncate(path.c_str(), newSize);
    }
    while ( ret == EINTR );

    if(ret != 0)
        throwCurrentErrno("truncate", path);
}


void FileImpl::remove(const std::string& path)
{
    if(0 != ::remove(path.c_str()))
        throwCurrentErrno("remove", path);
}


void FileImpl::move(const std::string& path, const std::string& to)
{
    if( 0 != ::rename(path.c_str(), to.c_str()) )
        throwCurrentErrno("rename", path);
}


void FileImpl::link(const std::string& path, const std::string& to)
{
    if( 0 != ::link(path.c_str(), to.c_str()) )
        throwCurrentErrno("link", path);
}


void FileImpl::symlink(const std::string& path, const std::string& to)
{
    if( 0 != ::symlink(path.c_str(), to.c_str()) )
        throwCurrentErrno("symlink", path);
}


void FileImpl::copy(const std::string& path, const std::string& to)
{
    int srcFd = -1;
    int dstFd = -1;

    try
    {
        srcFd = ::open(path.c_str(), O_RDWR|O_EXCL, 0777);
        if( srcFd < 0 )
            throwCurrentErrno("open", path);

        dstFd = ::open(to.c_str(), O_WRONLY|O_EXCL|O_CREAT, 0777);
        if( dstFd < 0 )
            throwCurrentErrno("open", to);

#if defined(HAVE_SENDFILE) && defined(HAVE_SYS_SENDFILE_H)
        struct stat buff;
        if( fstat(srcFd, &buff) != 0 )
            throwCurrentErrno("stat", path);

        while (::sendfile(dstFd, srcFd, 0, buff.st_size) == -1)
        {
            if (errno != EINTR)
                throwCurrentErrno("sendfile", path);
        }

#else
        char buffer[8192];

        while (true)
        {
            ssize_t n = ::read(srcFd, buffer, sizeof(buffer));
            if (n == 0)
                break;

            if (n < 0)
            {
                if (errno == EINTR)
                    continue;

                throwCurrentErrno("read", path);
            }

            const char* p = buffer;
            do
            {
                ssize_t nn = ::write(dstFd, p, n);
                if (nn < 0)
                {
                    if (errno == EINTR)
                        continue;

                    throwCurrentErrno("write", to);
                }

                p += nn;
                n -= nn;
            } while (n > 0);
        }

#endif

        ::close(srcFd);
        ::close(dstFd);
    }
    catch (...)
    {
        if (srcFd != -1)
            ::close(srcFd);
        if (dstFd != -1)
            ::close(dstFd);
        throw;
    }
}


void FileImpl::create(const std::string& path)
{
    int fd = ::open(path.c_str(), O_RDWR|O_EXCL|O_CREAT, 0777);
    if( fd < 0 )
        throwCurrentErrno("open", path);

    ::close(fd);
}

}
