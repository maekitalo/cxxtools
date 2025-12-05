/*
 * Copyright (C) 2005 by Marc Boris Drner
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
#include "filedeviceimpl.h"
#include <cxxtools/iodevice.h>
#include <cxxtools/datetime.h>
#include "error.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "config.h"

namespace cxxtools
{

FileDeviceImpl::FileDeviceImpl(FileDevice& device)
: IODeviceImpl(device)
{ }


FileDeviceImpl::~FileDeviceImpl()
{ }


bool FileDeviceImpl::seekable() const
{
    struct stat s;

    int ret = fstat(_fd, &s);
    if(ret == 0)
    {
        if(S_ISREG(s.st_mode) || S_ISBLK(s.st_mode))
            return true;
    }

    return false;
}


FileDeviceImpl::pos_type FileDeviceImpl::seek(off_type offset, std::ios::seekdir sd )
{
    int whence = std::ios::cur;
    switch(sd)
    {
        case std::ios::beg:
            whence = SEEK_SET;
            break;

        case std::ios::cur:
            whence = SEEK_CUR;
            break;

        case std::ios::end:
            whence = SEEK_END;
            break;

        default:
            break;
    }

    off_t ret = lseek(fd(), offset, whence);
    if( ret == (off_t)-1 )
        throw IOError(getErrnoString("lseek"));

    return ret;
}


void FileDeviceImpl::resize(off_type size)
{
    int ret = ::ftruncate(fd(), size);
    if(ret != 0)
        throw IOError(getErrnoString("ftruncate"));

}


FileDevice::Stat FileDeviceImpl::stat() const
{
    FileDevice::Stat result;
#ifdef HAVE_STATX
    struct statx st;
    int ret = statx(fd(), "", AT_EMPTY_PATH, STATX_SIZE|STATX_MTIME|STATX_BTIME, &st);
    if(ret != 0)
        throw IOError(getErrnoString("fstatx"));

    result.size = st.stx_size;
    result.mtime = DateTime::fromMSecsSinceEpoch(
                    Seconds(st.stx_mtime.tv_sec)
                        + Microseconds(st.stx_mtime.tv_nsec / 1000));
    result.ctime = DateTime::fromMSecsSinceEpoch(
                    Seconds(st.stx_btime.tv_sec)
                        + Microseconds(st.stx_btime.tv_nsec / 1000));
#else
    struct stat st;
    int ret = fstat(fd(), &st);
    if(ret != 0)
        throw IOError(getErrnoString("fstat"));

    result.size = st.st_size;
    result.mtime = DateTime::fromMSecsSinceEpoch(
                    Seconds(st.st_mtim.tv_sec)
                        + Microseconds(st.st_mtim.tv_nsec / 1000));
    result.ctime = DateTime::fromMSecsSinceEpoch(
                    Seconds(st.st_ctim.tv_sec)
                        + Microseconds(st.st_ctim.tv_nsec / 1000));
#endif
    return result;
}

size_t FileDeviceImpl::peek(char* buffer, size_t count)
{
    bool eof;
    size_t ret = this->read(buffer, count, eof);

    // if we could read data seek back
    if(ret > 0)
        this->seek(-((off_type)ret), std::ios::cur);

    return ret;
}


void FileDeviceImpl::sync() const
{
    int ret = fsync(_fd);
    if(ret != 0)
        throw IOError(getErrnoString("fsync"));
}


} //namespace cxxtools
