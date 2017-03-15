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

#include "cxxtools/iodevice.h"
#include "iodeviceimpl.h"

namespace cxxtools
{

IODevice::IODevice()
: _eof(false)
, _async(false)
, _rbuf(0)
, _rbuflen(0)
, _ravail(0)
, _wbuf(0)
, _wbuflen(0)
, _wavail(0)
, _reserved(0)
{ }

size_t IODevice::onBeginRead(char* buffer, size_t n, bool& eof)
{
    return ioimpl().beginRead(buffer, n, eof);
}

size_t IODevice::onEndRead(bool& eof)
{
    return ioimpl().endRead(eof);
}

size_t IODevice::onRead(char* buffer, size_t count, bool& eof)
{
    return ioimpl().read( buffer, count, eof );
}

size_t IODevice::onBeginWrite(const char* buffer, size_t n)
{
    return ioimpl().beginWrite(buffer, n);
}

size_t IODevice::onEndWrite()
{
    return ioimpl().endWrite();
}

size_t IODevice::onWrite(const char* buffer, size_t count)
{
    return ioimpl().write(buffer, count);
}

void IODevice::onClose()
{
    cancel();
    ioimpl().close();
}

void IODevice::onCancel()
{
    ioimpl().cancel();
}

void IODevice::onSync()
{
    ioimpl().sync();
}

void IODevice::onAttach(SelectorBase& s)
{
    ioimpl().attach(s);
}

void IODevice::onDetach(SelectorBase& s)
{
    ioimpl().detach(s);
}

void IODevice::beginRead(char* buffer, size_t n)
{
    if (!async())
        throw std::logic_error("Device not in async mode");

    if (!enabled())
        throw DeviceClosed("Device closed");

    if (_rbuf)
        throw IOPending("read operation pending");

    size_t r = this->onBeginRead(buffer, n, _eof);

    if (r > 0 || _eof || _wavail)
        this->setState(Selectable::Avail);
    else
        this->setState(Selectable::Busy);

    _rbuf = buffer;
    _rbuflen = n;
    _ravail = r;
}


size_t IODevice::endRead()
{
    if ( ! _rbuf )
        return 0;

    size_t n;
    try
    {
        n = this->onEndRead(_eof);
    }
    catch (...)
    {
        _rbuf = 0;
        _rbuflen = 0;
        _ravail = 0;
        throw;
    }

    if (_wavail > 0)
        this->setState(Selectable::Avail);
    else if (_wbuf)
        this->setState(Selectable::Busy);
    else
        this->setState(Selectable::Idle);

    _rbuf = 0;
    _rbuflen = 0;
    _ravail = 0;

    return n;
}


size_t IODevice::read(char* buffer, size_t n)
{
    if (async())
    {
        if ( _rbuf )
            throw IOPending("read operation pending");

        try // TODO pass buffer pointer/length to onEndRead
        {
            this->beginRead(buffer, n);
            size_t n = this->onEndRead(_eof);
            _rbuf = 0; _rbuflen = 0; _ravail = 0;
            return n;
        }
        catch(...)
        {
            _rbuf = 0; _rbuflen = 0; _ravail = 0;
            throw;
        }
    }

    return this->onRead(buffer, n, _eof);
}


size_t IODevice::beginWrite(const char* buffer, size_t n)
{
    if (!async())
        throw std::logic_error("Device not in async mode");

    if (!enabled())
        throw std::logic_error("Device not enabled");

    if (_wbuf)
        throw IOPending("write operation pending");

    size_t r = this->onBeginWrite(buffer, n);

    if (r > 0 || _ravail)
        this->setState(Selectable::Avail);
    else
        this->setState(Selectable::Busy);

    _wbuf = buffer;
    _wbuflen = n;
    _wavail = r;

    return r;
}


size_t IODevice::endWrite()
{
    if ( ! _wbuf )
        return 0;

    size_t n;
    try
    {
        n = onEndWrite();
    }
    catch (...)
    {
        _wbuf = 0;
        _wbuflen = 0;
        _wavail = 0;
        throw;
    }

    if (_ravail > 0 || (_rbuf && _eof) )
        this->setState(Selectable::Avail);
    else if (_rbuf)
        this->setState(Selectable::Busy);
    else
        this->setState(Selectable::Idle);

    _wbuf = 0;
    _wbuflen = 0;
    _wavail = 0;

    return n;
}


size_t IODevice::write(const char* buffer, size_t n)
{
    if ( async() )
    {
        if ( _wbuf )
        {
            throw IOPending("write operation pending");
        }

        try
        {
            this->beginWrite(buffer, n);
            size_t c = endWrite();
            _wbuf = 0; _wbuflen = 0; _wavail = 0;
            return c;
        }
        catch(...)
        {
            _wbuf = 0; _wbuflen = 0; _wavail = 0;
            throw;
        }
    }

    return this->onWrite(buffer, n);
}


void IODevice::cancel()
{
    onCancel();

    setState(Selectable::Idle);

    _rbuf = 0;
    _rbuflen = 0;
    _ravail = 0;

    _wbuf = 0;
    _wbuflen = 0;
    _wavail = 0;
}


bool IODevice::seekable() const
{
    return onSeekable();
}


IODevice::pos_type IODevice::seek(off_type offset, std::ios::seekdir sd)
{
    off_type ret = this->onSeek(offset, sd);
    if ( ret != off_type(-1) )
        setEof(false);

    return ret;
}


size_t IODevice::peek(char* buffer, size_t n)
{
    return this->onPeek(buffer, n);
}


void IODevice::sync()
{
    return this->onSync();
}


IODevice::pos_type IODevice::position()
{
    return this->seek(0, std::ios::cur);
}


bool IODevice::eof() const
{
    return _eof;
}


bool IODevice::async() const
{
    return _async;
}


void IODevice::setEof(bool eof)
{
    _eof = eof;
}


void IODevice::setAsync(bool async)
{
    _async = async;
}

void IODevice::setTimeout(Milliseconds timeout)
{
    ioimpl().setTimeout(timeout);
}

}
