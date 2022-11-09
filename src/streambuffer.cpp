/*
 * Copyright (C) 2004-2008 Marc Boris Duerner
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

#include "cxxtools/streambuffer.h"
#include <algorithm>
#include <stdexcept>
#include <cstring>
#include <cxxtools/log.h>

log_define("cxxtools.streambuffer")

namespace cxxtools {

StreamBuffer::StreamBuffer(IODevice& ioDevice, size_t bufferSize, bool extend)
: _ioDevice(&ioDevice),
  _ibufferSize(bufferSize+4),
  _ibuffer(0),
  _obufferSize(bufferSize),
  _obuffer(0),
  _pbmax(4),
  _oextend(extend)
{
    setg(0, 0, 0);
    setp(0, 0);

    attach(ioDevice);
}


StreamBuffer::StreamBuffer(size_t bufferSize, bool extend)
: _ioDevice(0),
  _ibufferSize(bufferSize+4),
  _ibuffer(0),
  _obufferSize(bufferSize),
  _obuffer(0),
  _pbmax(4),
  _oextend(extend)
{
    setg(0, 0, 0);
    setp(0, 0);
}


StreamBuffer::~StreamBuffer()
{
    delete[] _ibuffer;
    delete[] _obuffer;
}


void StreamBuffer::attach(IODevice& ioDevice)
{
    if (ioDevice.busy())
        throw IOPending("IODevice in use");

    detach();
    _ioDevice = &ioDevice;
    connect(ioDevice.inputReady, *this, &StreamBuffer::onRead);
    connect(ioDevice.outputReady, *this, &StreamBuffer::onWrite);
}

void StreamBuffer::detach()
{
    if (_ioDevice)
    {
        if (_ioDevice->busy())
            throw IOPending("IODevice in use");

        disconnect(_ioDevice->inputReady, *this, &StreamBuffer::onRead);
        disconnect(_ioDevice->outputReady, *this, &StreamBuffer::onWrite);
        _ioDevice = nullptr;
    }
}

void StreamBuffer::beginRead()
{
    if (_ioDevice == 0 || _ioDevice->reading())
        return;

    if (! _ibuffer)
    {
        _ibuffer = new char[_ibufferSize];
    }

    size_t putback = _pbmax;
    size_t leftover = 0;

    // keep chars for putback
    if (gptr())
    {
        putback = std::min<size_t>( gptr() - eback(), _pbmax);
        char* to = _ibuffer + _pbmax - putback;
        char* from = gptr() - putback;

        leftover = egptr() - gptr();
        std::memmove( to, from, putback + leftover );
    }

    size_t used = _pbmax + leftover;

    if (_ibufferSize == used)
        throw std::logic_error("StreamBuffer is full");

    _ioDevice->beginRead( _ibuffer + used, _ibufferSize - used );

    setg( _ibuffer + (_pbmax - putback), // start of get area
                _ibuffer + putback, // gptr position
                _ibuffer + used ); // end of get area
}


void StreamBuffer::onRead(IODevice& /*dev*/)
{
    inputReady.send(*this);
}


size_t StreamBuffer::endRead()
{
    size_t readSize = _ioDevice->endRead();

    setg(eback(), // start of get area
         gptr(),  // gptr position
         egptr() + readSize); // end of get area

    return readSize;
}


StreamBuffer::int_type StreamBuffer::underflow()
{
    log_trace("underflow");

    if (!_ioDevice)
        return traits_type::eof();

    if (_ioDevice->reading())
        endRead();

    if (gptr() < egptr())
        return traits_type::to_int_type( *(gptr()) );

    if (_ioDevice->eof())
        return traits_type::eof();

    if (! _ibuffer)
    {
        _ibuffer = new char[_ibufferSize];
    }

    size_t putback = _pbmax;

    if (gptr())
    {
        putback = std::min<size_t>(gptr() - eback(), _pbmax);
        std::memmove( _ibuffer + (_pbmax - putback),
                      gptr() - putback,
                      putback );
    }

    size_t readSize = _ioDevice->read( _ibuffer + _pbmax, _ibufferSize - _pbmax );

    setg( _ibuffer + _pbmax - putback,    // start of get area
                _ibuffer + _pbmax,              // gptr position
                _ibuffer + _pbmax + readSize ); // end of get area

    if (_ioDevice->eof())
        return traits_type::eof();

    return traits_type::to_int_type( *(gptr()) );
}


size_t StreamBuffer::beginWrite()
{
    log_trace("beginWrite; out_avail=" << out_avail());

    if (_ioDevice == 0)
        return 0;

    if (_ioDevice->writing())
    {
        log_debug("device already in write mode - nothing done");
        return 0;
    }

    if (pptr())
    {
        size_t avail = pptr() - pbase();
        if (avail > 0)
        {
            return _ioDevice->beginWrite(_obuffer, avail);
        }
    }

    return 0;
}


void StreamBuffer::discard()
{
    if (_ioDevice && (_ioDevice->reading() || _ioDevice->writing()))
        throw IOPending("discard failed - streambuffer is in use");

    if (gptr())
        setg(_ibuffer, _ibuffer + _ibufferSize, _ibuffer + _ibufferSize);

    if (pptr())
        setp(_obuffer, _obuffer + _obufferSize);
}


void StreamBuffer::onWrite(IODevice& /*dev*/)
{
    outputReady.send(*this);
}


size_t StreamBuffer::endWrite()
{
    log_trace("endWrite; out_avail=" << out_avail());

    size_t leftover = 0;
    size_t written = 0;

    if (pptr())
    {
        size_t avail = pptr() - pbase();
        written = _ioDevice->endWrite();

        leftover = avail - written;

        log_debug(written << " bytes written; " << leftover << " left in buffer");

        if (leftover > 0)
        {
            traits_type::move(_obuffer, _obuffer + written, leftover);
        }
    }

    setp(_obuffer, _obuffer + _obufferSize);
    pbump( leftover );

    return written;
}


StreamBuffer::int_type StreamBuffer::overflow(int_type ch)
{
    log_trace("overflow(" << ch << ')');

    if (!_ioDevice)
        return traits_type::eof();

    if (!_obuffer)
    {
        _obuffer = new char[_obufferSize];
        setp(_obuffer, _obuffer + _obufferSize);
    }
    else if (_oextend && !traits_type::eq_int_type( ch, traits_type::eof() ))
    {
        // break asyncronous I/O if any active
        bool pendingWrite = _ioDevice->writing();
        bool pendingRead = false;

        if (pendingWrite)
        {
            if (_ioDevice->wavail())
            {
                log_debug("finish writing");
                endWrite();
            }
            else
            {
                pendingRead = _ioDevice->reading();
                log_debug("cancel asyncronous operations");
                _ioDevice->cancel();
            }
        }

        // `endWrite` may have freed some data in the buffer, in which case a
        // expand is not needed any more.

        if (epptr() == pptr())
        {
            // if the buffer area is extensible and overflow is not called by
            // sync/flush we copy the output buffer to a larger one
            size_t bufsize = _obufferSize + (_obufferSize/2);
            log_debug("extend buffer from " << _obufferSize << " to " << bufsize);
            char* buf = new char[ bufsize ];
            traits_type::copy(buf, _obuffer, _obufferSize);
            std::swap(_obuffer, buf);
            setp(_obuffer, _obuffer + bufsize);
            pbump( _obufferSize );
            _obufferSize = bufsize;
            delete [] buf;
        }

        // restart asyncronous I/O
        if (pendingWrite)
        {
            beginWrite();
            if (pendingRead)
                beginRead();
        }
    }
    else if (_ioDevice->writing()) // beginWrite is unfinished
    {
        log_debug("finish writing");
        endWrite();
    }
    else
    {
        // normal blocking overflow case
        log_debug("blocking overflow");
        size_t avail = pptr() - _obuffer;
        size_t written = _ioDevice->write(_obuffer, avail);
        size_t leftover = avail - written;

        if (leftover > 0)
        {
            traits_type::move(_obuffer, _obuffer + written, leftover);
        }

        setp(_obuffer, _obuffer + _obufferSize);
        pbump( leftover );
    }

    // if the overflow char is not EOF put it in buffer
    if (traits_type::eq_int_type(ch, traits_type::eof()) ==  false)
    {
        *pptr() = traits_type::to_char_type(ch);
        pbump(1);
    }

    return traits_type::not_eof(ch);
}


StreamBuffer::int_type StreamBuffer::pbackfail(StreamBuffer::int_type)
{
    return traits_type::eof();
}


int StreamBuffer::sync()
{
    log_trace("sync");

    if (! _ioDevice)
        return 0;

    if (pptr())
    {
        while (pptr() > pbase())
        {
            const int_type ch = overflow( traits_type::eof() );
            if (ch == traits_type::eof())
            {
                return -1;
            }

            _ioDevice->sync();
        }
    }

    return 0;
}


std::streamsize StreamBuffer::xspeekn(char* buffer, std::streamsize size)
{
    if (traits_type::eof() == underflow())
        return 0;

    const std::streamsize avail = egptr() - gptr();
    size = std::min(avail, size);
    if (size == 0)
        return 0;

    std::memcpy(buffer, gptr(), sizeof(char) * size);
    return size;
}


StreamBuffer::pos_type
StreamBuffer::seekoff(off_type off, std::ios::seekdir dir, std::ios::openmode)
{
    pos_type ret =  pos_type( off_type(-1) );

    if (! _ioDevice || ! _ioDevice->enabled() ||
         ! _ioDevice->seekable() || off == 0)
    {
        return ret;
    }

    if (_ioDevice->writing())
    {
        endWrite();
    }

    if (_ioDevice->reading())
    {
        endRead();
    }

    ret = _ioDevice->seek(off, dir);

    // eliminate currently buffered sequence
    discard();

    return ret;
}


StreamBuffer::pos_type
StreamBuffer::seekpos(pos_type p, std::ios::openmode mode)
{
    return seekoff(p, std::ios::beg, mode);
}

}
