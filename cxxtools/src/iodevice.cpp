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
#include <string.h>

namespace cxxtools {

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


IODevice::~IODevice()
{ }


void IODevice::beginRead(char* buffer, size_t n)
{
    if ( ! async() )
        throw std::logic_error( CXXTOOLS_ERROR_MSG("Device not in async mode") );

    if(_rbuf)
        throw IOPending( CXXTOOLS_ERROR_MSG("read operation pending") );

    size_t r = this->onBeginRead(buffer, n, _eof);

    if(r > 0 || _eof || _wavail)
        this->setState(Selectable::Avail);
    else
        this->setState(Selectable::Busy);

    _rbuf = buffer;
    _rbuflen = n;
    _ravail = r;
}


size_t IODevice::endRead()
{
    if( ! _rbuf )
        return 0;

    size_t n = this->onEndRead(_eof);

    if(_wavail > 0)
        this->setState(Selectable::Avail);
    else if(_wbuf)
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
    if( async() )
    {
        if( ! _rbuf )
        {
            try // TODO pass buffer pointer/length to onEndRead
            {
                this->beginRead(buffer, n);
                return this->onEndRead(_eof);
            }
            catch(...)
            {
                _rbuf = 0; _rbuflen = 0; _ravail = 0;
                throw;
            }
        }

        size_t available = this->onEndRead(_eof);
        size_t transferred = std::min(n, available);
        size_t leftover = available > n ? available - n : 0;

        memcpy( buffer, _rbuf, transferred );
        if(leftover > 0)
            memmove(_rbuf, _rbuf+transferred, leftover);

        _ravail = leftover + this->onBeginRead(_rbuf+leftover, _rbuflen-leftover, _eof);

        if(_ravail || _eof || _wavail)
            this->setState(Selectable::Avail);
        else
            this->setState(Selectable::Busy);

        return transferred;
    }

    return this->onRead(buffer, n, _eof);
}


void IODevice::beginWrite(const char* buffer, size_t n)
{
    if ( ! async() )
        throw std::logic_error( CXXTOOLS_ERROR_MSG("Device not in async mode") );

    if(_wbuf)
        throw IOPending( CXXTOOLS_ERROR_MSG("write operation pending") );

    size_t r = this->onBeginWrite(buffer, n);

    if(r > 0 || _ravail)
        this->setState(Selectable::Avail);
    else
        this->setState(Selectable::Busy);

    _wbuf = buffer;
    _wbuflen = n;
    _wavail = r;
}


size_t IODevice::endWrite()
{
    if( ! _wbuf )
        return 0;

    size_t n =  onEndWrite();

    if(_ravail > 0 || (_rbuf && _eof) )
        this->setState(Selectable::Avail);
    else if(_rbuf)
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
    if( async() )
    {
        if( _wbuf )
        {
            return this->endWrite();
        }

        try
        {
            this->beginWrite(buffer, n);
            return endWrite();
        }
        catch(...)
        {
            _wbuf = 0; _wbuflen = 0; _wavail = 0;
            throw;
        }
    }

    return this->onWrite(buffer, n);
}


bool IODevice::seekable() const
{
    return onSeekable();
}


IODevice::pos_type IODevice::seek(off_type offset, std::ios::seekdir sd)
{
    off_type ret = this->onSeek(offset, sd);
    if( ret != off_type(-1) )
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

}
