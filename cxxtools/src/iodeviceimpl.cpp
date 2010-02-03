/*
 * Copyright (C) 2006-2007 Laurentiu-Gheorghe Crisan
 * Copyright (C) 2006-2007 Marc Boris Duerner
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
#include "iodeviceimpl.h"
#include "cxxtools/ioerror.h"
#include "error.h"
#include <cerrno>
#include <cassert>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/poll.h>
#include <cxxtools/log.h>

log_define("cxxtools.iodevice.impl")

namespace cxxtools {

const short IODeviceImpl::POLLERR_MASK= POLLERR | POLLHUP | POLLNVAL;
const short IODeviceImpl::POLLIN_MASK= POLLIN;
const short IODeviceImpl::POLLOUT_MASK= POLLOUT;

IODeviceImpl::IODeviceImpl(IODevice& device)
: _device(device)
, _fd(-1)
, _timeout(Selectable::WaitInfinite)
, _pfd(0)
, _sentry(0)
{ }



IODeviceImpl::~IODeviceImpl()
{
    assert(_pfd == 0);

    if(_sentry)
        _sentry->detach();
}


void IODeviceImpl::open(const std::string& path, IODevice::OpenMode mode)
{
    int flags = O_RDONLY;

    if( (mode & IODevice::Read ) && (mode & IODevice::Write) )
    {
        flags |= O_RDWR;
    }
    else if(mode & IODevice::Write)
    {
        flags |= O_WRONLY;
    }
    else if(mode & IODevice::Read  )
    {
        flags |= O_RDONLY;
    }

    if(mode & IODevice::Async)
        flags |= O_NONBLOCK;

    if(mode & IODevice::Trunc)
        flags |= O_TRUNC;

    flags |=  O_NOCTTY;

    _fd = ::open( path.c_str(), flags );
    if(_fd == -1)
        throw AccessFailed(getErrnoString("open failed"), CXXTOOLS_SOURCEINFO);
}


void IODeviceImpl::open(int fd, bool isAsync, bool inherit)
{
    _fd = fd;

    if (isAsync)
    {
        int flags = fcntl(_fd, F_GETFL);
        flags |= O_NONBLOCK ;
        int ret = fcntl(_fd, F_SETFL, flags);
        if(-1 == ret)
            throw IOError(getErrnoString("Could not set fd to non-blocking"), CXXTOOLS_SOURCEINFO);
    }

    if (!inherit)
    {
        int flags = fcntl(_fd, F_GETFD);
        flags |= FD_CLOEXEC ;
        int ret = fcntl(_fd, F_SETFD, flags);
        if(-1 == ret)
            throw IOError(getErrnoString("Could not set FD_CLOEXEC"), CXXTOOLS_SOURCEINFO);
    }

}


void IODeviceImpl::close()
{
    log_debug("close device; fd=" << _fd << " pfd=" << _pfd);

    if(_fd != -1)
    {
        int fd = _fd;
        _fd = -1;
        _pfd = 0;

        while ( ::close(fd) != 0 )
        {
            if( errno != EINTR )
            {
                log_error("close of iodevice failed; errno=" << errno);
                throw IOError(getErrnoString("Could not close file handle"), CXXTOOLS_SOURCEINFO);
            }
        }
    }
}


size_t IODeviceImpl::beginRead(char* buffer, size_t n, bool& eof)
{
    if(_pfd)
    {
        _pfd->events |= POLLIN;
    }

    return 0;
}


size_t IODeviceImpl::endRead(bool& eof)
{
    if(_pfd)
    {
        _pfd->events &= ~POLLIN;
    }

    return this->read( _device.rbuf(), _device.rbuflen(), eof );
}


size_t IODeviceImpl::read( char* buffer, size_t count, bool& eof )
{
    ssize_t ret = 0;

    while(true)
    {
        ret = ::read( _fd, (void*)buffer, count);

        if(ret > 0)
        {
            log_debug("::read(" << _fd << ", " << count << ") returned " << ret << " => \"" << std::string(buffer, ret) << "\")");
            break;
        }

        log_debug("::read(" << _fd << ", " << count << ") returned " << ret << " errno=" << errno);

        if(ret == 0 || errno == ECONNRESET)
        {
            eof = true;
            return 0;
        }

        if(errno == EINTR)
            continue;

        if(errno != EAGAIN)
            throw IOError(getErrnoString("read failed"), CXXTOOLS_SOURCEINFO);

        pollfd pfd;
        pfd.fd = this->fd();
        pfd.revents = 0;
        pfd.events = POLLIN;

        bool ret = this->wait(_timeout, pfd);
        if(false == ret)
        {
            log_debug("timeout");
            throw IOTimeout();
        }
    }

    return ret;
}


size_t IODeviceImpl::beginWrite(const char* buffer, size_t n)
{
    if(_pfd)
    {
        _pfd->events |= POLLOUT;
    }

    return 0;
}


size_t IODeviceImpl::endWrite()
{
    if(_pfd)
    {
        _pfd->events &= ~POLLOUT;
    }

    return this->write( _device.wbuf(), _device.wbuflen() );
}


size_t IODeviceImpl::write( const char* buffer, size_t count )
{
    ssize_t ret = 0;

    while(true)
    {
        log_debug("::write(" << _fd << ", buffer, " << count << ')');

        ret = ::write(_fd, (const void*)buffer, count);
        log_debug("write returned " << ret);
        if(ret > 0)
            break;

        if(ret == 0 || errno == ECONNRESET || errno == EPIPE)
            throw IOError("lost connection to peer");

        if(errno == EINTR)
            continue;

        if(errno != EAGAIN)
            throw IOError(getErrnoString("Could not write to file handle"), CXXTOOLS_SOURCEINFO);

        pollfd pfd;
        pfd.fd = this->fd();
        pfd.revents = 0;
        pfd.events = POLLOUT;

        if (!this->wait(_timeout, pfd))
        {
            throw IOTimeout();
        }
    }

    return ret;
}


void IODeviceImpl::cancel()
{
    if(_pfd)
    {
        _pfd->events &= ~(POLLIN|POLLOUT);
    }
}


void IODeviceImpl::sync() const
{
    int ret = fsync(_fd);
    if(ret != 0)
        throw IOError(getErrnoString("Could not sync handle"), CXXTOOLS_SOURCEINFO);
}


void IODeviceImpl::attach(SelectorBase& s)
{

}


void IODeviceImpl::detach(SelectorBase& s)
{
    _pfd = 0;
}


bool IODeviceImpl::wait(std::size_t msecs)
{
    pollfd pfd;
    this->initWait(pfd);
    this->wait(msecs, pfd);
    return this->checkPollEvent(pfd);
}


bool IODeviceImpl::wait(std::size_t umsecs, pollfd& pfd)
{
    int msecs = static_cast<int>(umsecs);

    if( umsecs > std::numeric_limits<int>::max() )
    {
        if(umsecs == SelectorBase::WaitInfinite)
            msecs = -1;
        else
            msecs = std::numeric_limits<int>::max();
    }

    int ret = -1;
    do
    {
        ret = ::poll(&pfd, 1, msecs);
    } while (ret == -1 && errno == EINTR);

    if (ret == -1)
        throw IOError(getErrnoString("poll failed"), CXXTOOLS_SOURCEINFO );

    return ret > 0;
}


void IODeviceImpl::initWait(pollfd& pfd)
{
    pfd.fd = this->fd();
    pfd.revents = 0;
    pfd.events = 0;

    if( _device.rbuf() )
        pfd.events |= POLLIN;
    if( _device.wbuf() )
        pfd.events |= POLLOUT;
}


size_t IODeviceImpl::initializePoll(pollfd* pfd, size_t pollSize)
{
    assert(pfd != 0);
    assert(pollSize >= 1);

    this->initWait(*pfd);
    _pfd = pfd;

	return 1;
}


bool IODeviceImpl::checkPollEvent()
{
    // _pfd can be 0 if the device is just added during wait iteration
    if (_pfd == 0)
        return false;

    return this->checkPollEvent(*_pfd);
}


bool IODeviceImpl::checkPollEvent(pollfd& pfd)
{
    log_trace("checkPollEvent");

    bool avail = false;

    DestructionSentry sentry(_sentry);

    if (pfd.revents & POLLERR_MASK)
    {
        log_debug("send signal errorOccured");
        _device.errorOccured(_device);
        avail = true;
    }

    if( ! _sentry )
        return avail;

    if( pfd.revents & POLLOUT_MASK )
    {
        log_debug("send signal outputReady");
        _device.outputReady(_device);
        avail = true;
    }

    if( ! _sentry )
        return avail;

    if( pfd.revents & POLLIN_MASK )
    {
        log_debug("send signal inputReady");
        _device.inputReady(_device);
        avail = true;
    }

    return avail;
}

}
