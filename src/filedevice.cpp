/*
 * Copyright (C) 2006-2007 Marc Boris Duerner
 * Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan
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
#include <cxxtools/filedevice.h>
#include "filedeviceimpl.h"

namespace cxxtools
{

FileDevice::FileDevice()
{
    _impl = new FileDeviceImpl(*this);
}


FileDevice::FileDevice(const std::string& path, IODevice::OpenMode mode, bool inherit)
{
    _impl = new FileDeviceImpl(*this);

    open(path, mode, inherit);
}


FileDevice::~FileDevice()
{
    try
    {
        close();
    }
    catch(...)
    { }

    delete _impl;
}


void FileDevice::open( const std::string& path, IODevice::OpenMode mode, bool inherit)
{
    close();
    _impl->open(path, mode, inherit);
    _path = path;
}


void FileDevice::onClose()
{
    _impl->close();
}


void FileDevice::onSetTimeout(Timespan timeout)
{
    _impl->setTimeout(timeout);
}


size_t FileDevice::onBeginRead(char* buffer, size_t n, bool& eof)
{
    return _impl->beginRead(buffer, n, eof);
}


size_t FileDevice::onEndRead(bool& eof)
{
    return _impl->endRead(eof);
}


size_t FileDevice::onBeginWrite(const char* buffer, size_t n)
{
    return _impl->beginWrite(buffer, n);
}


size_t FileDevice::onEndWrite()
{
    return _impl->endWrite();
}


size_t FileDevice::size() const
{
    return _impl->size();
}


FileDevice::pos_type FileDevice::onSeek(off_type offset, std::ios::seekdir sd)
{
    return _impl->seek(offset, sd);
}


size_t FileDevice::onRead( char* buffer, size_t count, bool& eof )
{
    //if(count > SSIZE_MAX)
    //    count = SSIZE_MAX;

    size_t ret = _impl->read( buffer, count, eof );
    return ret;
}


size_t FileDevice::onWrite(const char* buffer, size_t count)
{
    return _impl->write(buffer, count);
}


void FileDevice::onCancel()
{
    _impl->cancel();
}


size_t FileDevice::onPeek(char* buffer, size_t count)
{
    return _impl->peek(buffer, count);
}


void FileDevice::onSync() const
{
    _impl->sync();
}


} // namespace cxxtools
