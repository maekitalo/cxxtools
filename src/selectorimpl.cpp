/*
 * Copyright (C) 2006-2008 by Marc Boris Duerner
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

#include "selectorimpl.h"
#include "selectableimpl.h"
#include "cxxtools/ioerror.h"
#include "cxxtools/systemerror.h"
#include "cxxtools/selector.h"
#include "cxxtools/log.h"
#include <cerrno>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <cassert>
#include <iostream>
#include <limits>
#include "config.h"
#include "poll.h"

log_define("cxxtools.selector.impl")

namespace cxxtools
{

const short SelectorImpl::POLL_ERROR_MASK= POLLERR | POLLHUP | POLLNVAL;

SelectorImpl::SelectorImpl()
: _isDirty(true)
{
    _current = _devices.end();

    //Open a pipe to send wake up message.
    if( ::pipe( _wakePipe ) )
        throwSystemError("pipe");

    int flags = ::fcntl(_wakePipe[0], F_GETFL);
    if(-1 == flags)
        throwSystemError("fcntl");

    int ret = ::fcntl(_wakePipe[0], F_SETFL, flags|O_NONBLOCK);
    if(-1 == ret)
        throwSystemError("fcntl");

    flags = ::fcntl(_wakePipe[1], F_GETFL);
    if(-1 == flags)
        throwSystemError("fcntl");

    ret = ::fcntl(_wakePipe[1], F_SETFL, flags|O_NONBLOCK);
    if(-1 == ret)
        throwSystemError("fcntl");

}


SelectorImpl::~SelectorImpl()
{
    std::set<Selectable*>::iterator it;
    while( _devices.size() )
    {
        it = _devices.begin();
        (*it)->setSelector(0);
    }

    if( _wakePipe[0] != -1 && _wakePipe[1] != -1 )
    {
        ::close(_wakePipe[0]);
        ::close(_wakePipe[1]);
    }
}


void SelectorImpl::add(Selectable& dev)
{
    _devices.insert(&dev);
    _isDirty = true;
}


void SelectorImpl::remove(Selectable& dev)
{
   std::set<Selectable*>::iterator it = _devices.find( &dev );
   if( it == _devices.end() )
        return;

    if (_current == _devices.end())
    {
        _devices.erase(it);
    }
    else if (*_current == *it)
    {
        _devices.erase(_current++);
    }
    else
    {
        _devices.erase(it);
    }

    _isDirty = true;
}


void SelectorImpl::changed( Selectable& s )
{
    if( s.avail() )
    {
        _avail.insert(&s);
    }
    else
    {
        _avail.erase(&s);
    }
}


bool SelectorImpl::waitUntil(Timespan until)
{
    if (!_avail.empty())
        until = Timespan(0);

    if (_isDirty)
    {
        _pollfds.clear();

        // Groesse neu berechnen
        size_t pollSize= 1;

        std::set<Selectable*>::iterator iter;
        for( iter= _devices.begin(); iter != _devices.end(); ++iter)
        {
            if( (*iter)->enabled() )
                pollSize+= (*iter)->simpl().pollSize();
        }

        pollfd pfd;
        pfd.fd = -1;
        pfd.events = 0;
        pfd.revents = 0;

        _pollfds.assign(pollSize, pfd);

        // Eintraege einfuegen
        pollfd* pCurr= &_pollfds[0];

        // Event Pipe einfuegen
        pCurr->fd = _wakePipe[0];
        pCurr->events = POLLIN;

        ++pCurr;

        for( iter= _devices.begin(); iter != _devices.end(); ++iter)
        {
            if( (*iter)->enabled() )
            {
                const size_t availableSpace= &_pollfds.back() - pCurr + 1;
                size_t required = (*iter)->simpl().pollSize();
                assert( required <= availableSpace);
                pCurr+= (*iter)->simpl().initializePoll( pCurr, required);
            }
        }

        _isDirty= false;
    }

#ifdef HAVE_PPOLL
    struct timespec pollTimeout = { 0, 0 };
    struct timespec* pollTimeoutP = 0;
    if (until >= Timespan(0))
        pollTimeoutP = &pollTimeout;
#else
    int pollTimeout = until == Timespan(0) ? 0 : -1;
#endif

    int ret = -1;
    while (true)
    {
        if (until > Timespan(0))
        {
            Timespan remaining = until - Timespan::gettimeofday();
            if (remaining < Timespan(0))
                remaining = Timespan(0);

#ifdef HAVE_PPOLL
            pollTimeout.tv_sec = remaining.totalUSecs() / 1000000;
            pollTimeout.tv_nsec = (remaining.totalUSecs() % 1000000) * 1000;
#else
            if (Milliseconds(remaining) >= std::numeric_limits<int>::max())
                pollTimeout = std::numeric_limits<int>::max();
            else
                pollTimeout = Milliseconds(remaining).ceil();
#endif

            log_debug("remaining " << remaining);
        }
        else
            log_debug("no timeout");

#ifdef HAVE_PPOLL
        log_debug("ppoll with " << _pollfds.size() << " fds, timeout=" << pollTimeout.tv_sec << "s " << pollTimeout.tv_nsec << "ns");
        ret = ::ppoll(&_pollfds[0], _pollfds.size(), pollTimeoutP, 0);
        log_debug("ppoll returns " << ret);
#else
        log_debug("poll with " << _pollfds.size() << " fds, timeout=" << pollTimeout << "ms");
        ret = ::poll(&_pollfds[0], _pollfds.size(), pollTimeout);
        log_debug("poll returns " << ret);
#endif
        if( ret != -1 )
            break;

        if( errno != EINTR )
            throw IOError("Could not poll on file descriptors");

    }

    if( ret == 0 && _avail.empty() )
        return false;

    bool avail = false;
    try
    {
        if (_pollfds[0].revents != 0)
        {

            if ( _pollfds[0].revents & POLL_ERROR_MASK)
            {
                throw IOError("poll error on event pipe");
            }

            static char buffer[1024];
            while(true)
            {
                int ret = ::read(_wakePipe[0], buffer, sizeof(buffer));
                if(ret > 0)
                {
                    avail = true;
                    continue;
                }

                if (ret == -1)
                {
                    if(errno == EINTR)
                        continue;

                    if(errno == EAGAIN)
                        break;
                }

                throw IOError("Could not read from pipe");
            }
        }

        for( _current = _devices.begin(); _current != _devices.end(); )
        {
            Selectable* dev = *_current;

            if ( dev->enabled() && dev->simpl().checkPollEvent() )
            {
                avail = true;
            }

            if (_current != _devices.end())
            {
                if (*_current == dev)
                {
                    ++_current;
                }
            }
        }
    }
    catch (...)
    {
        _current= _devices.end();
        throw;
    }

    return avail;
}


void SelectorImpl::wake()
{
    ::write( _wakePipe[1], "W", 1);
    ::fsync( _wakePipe[1] );
}

} //namespace cxxtools
