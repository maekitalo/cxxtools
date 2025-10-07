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
#include "config.h"

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

void PipeIODevice::open(int fd, bool isAsync, bool inherit)
{
    _impl.open(fd, isAsync, inherit);
    this->setEnabled(true);
    this->setAsync(isAsync);
    this->setEof(false);
}


PipeImpl::PipeImpl(bool isAsync, bool inherit)
{
    int fds[2];
#ifdef HAVE_PIPE2
    int flags = 0;
    if (isAsync)
        flags |= O_NONBLOCK;
    if (!inherit)
        flags |= O_CLOEXEC;

    if( ::pipe2(fds, flags) )
        throwSystemError("pipe2");

    _in.open(fds[0], isAsync, false);
    _out.open(fds[1], isAsync, false);
#else
    if(-1 == ::pipe(fds) )
        throw SystemError("pipe");

    _in.open(fds[0], isAsync, inherit);
    _out.open(fds[1], isAsync, inherit);
#endif
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
