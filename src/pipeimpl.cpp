/*
 * Copyright (C) 2006-2007 Marc Boris Duerner
 * Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan
 * Copyright (C) 2009 Tommi Maekitalo
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
#include "pipeimpl.h"
#include "cxxtools/systemerror.h"
#include <unistd.h>
#include <fcntl.h>

namespace cxxtools {

PipeIODevice::PipeIODevice()
: _impl(*this)
{
}


PipeIODevice::~PipeIODevice()
{
    try
    {
        IODevice::close();
    }
    catch(...)
    {}
}

void PipeIODevice::redirect(int newFd, bool close, bool inherit)
{
    int ret = ::dup2(fd(), newFd);
    if (ret < 0)
        throw SystemError("dup2");

    if (close)
    {
        IODevice::close();
        _impl.open(newFd, async(), inherit);
    }
}

void PipeIODevice::open(int fd, bool isAsync)
{
    _impl.open(fd, isAsync, true);
    this->setEnabled(true);
    this->setAsync(isAsync);
    this->setEof(false);
}


bool PipeIODevice::onWait(Timespan timeout)
{
    return _impl.wait(timeout);
}


size_t PipeIODevice::onBeginRead(char* buffer, size_t n, bool& eof)
{
    return _impl.beginRead(buffer, n, eof);
}


size_t PipeIODevice::onEndRead(bool& eof)
{
    return _impl.endRead(eof);
}


size_t PipeIODevice::onRead(char* buffer, size_t count, bool& eof)
{
    return _impl.read(buffer, count, eof);
}


size_t PipeIODevice::onBeginWrite(const char* buffer, size_t n)
{
    return _impl.beginWrite(buffer, n);
}


size_t PipeIODevice::onEndWrite()
{
    return _impl.endWrite();
}


size_t PipeIODevice::onWrite(const char* buffer, size_t count)
{
    return _impl.write(buffer, count);
}


void PipeIODevice::onCancel()
{
    _impl.cancel();
}


void PipeIODevice::onSync() const
{
    _impl.sync();
}


PipeImpl::PipeImpl(bool isAsync)
{
    int fds[2];
    if(-1 == ::pipe(fds) )
        throw SystemError("pipe");

    _out.open( fds[0], isAsync );
    _in.open( fds[1], isAsync );
}


PipeIODevice& PipeImpl::out()
{
    return _out;
}

const PipeIODevice& PipeImpl::out() const
{
    return _out;
}

PipeIODevice& PipeImpl::in()
{
    return _in;
}

const PipeIODevice& PipeImpl::in() const
{
    return _in;
}

void PipeImpl::redirectStdin(bool close, bool inherit)
{
    out().redirect(0, close, inherit);
}

void PipeImpl::redirectStdout(bool close, bool inherit)
{
    in().redirect(1, close, inherit);
}

void PipeImpl::redirectStderr(bool close, bool inherit)
{
    in().redirect(2, close, inherit);
}

}
