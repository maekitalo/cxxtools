/*
 * Copyright (C) 2005 Marc Boris Duerner
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
#include "cxxtools/iostream.h"

namespace cxxtools {

IStream::IStream(size_t bufferSize)
: _buffer(bufferSize)
{
    attachBuffer(&_buffer);
    connect(_buffer.inputReady, *this, &IStream::onInput);
}


IStream::IStream(IODevice& device, size_t bufferSize)
: _buffer(device, bufferSize)
{
    attachBuffer(&_buffer);
}


IStream::~IStream()
{
}

IODevice* IStream::attachDevice(IODevice& device)
{
    IODevice* ret = attachedDevice();
    _buffer.attach(device);
    return ret;
}

void IStream::endRead()
{
    buffer().endRead();
    if (buffer().in_avail() == 0)
        setstate(std::ios::eofbit);
}

void IStream::onInput(StreamBuffer&)
{
    inputReady(*this);
}


OStream::OStream(size_t bufferSize, bool extend)
: _buffer(bufferSize, extend)
{
    attachBuffer(&_buffer);
    connect(_buffer.outputReady, *this, &OStream::onOutput);
}


OStream::OStream(IODevice& device, size_t bufferSize, bool extend)
: _buffer(device, bufferSize, extend)
{
    attachBuffer(&_buffer);
}


OStream::~OStream()
{
}

IODevice* OStream::attachDevice(IODevice& device)
{
    IODevice* ret = attachedDevice();
    _buffer.attach(device);
    return ret;
}


void OStream::onOutput(StreamBuffer&)
{
    outputReady(*this);
}


IOStream::IOStream(size_t bufferSize, bool extend)
: _buffer(bufferSize, extend)
{
    attachBuffer(&_buffer);
    connect(_buffer.inputReady, *this, &IOStream::onInput);
    connect(_buffer.outputReady, *this, &IOStream::onOutput);
}


IOStream::~IOStream()
{
}


IOStream::IOStream(IODevice& device, size_t bufferSize, bool extend)
: _buffer(device, bufferSize, extend)
{
    attachBuffer(&_buffer);
}

void IOStream::onInput(StreamBuffer&)
{
    inputReady(*this);
}

void IOStream::onOutput(StreamBuffer&)
{
    outputReady(*this);
}

IODevice* IOStream::attachDevice(IODevice& device)
{
    IODevice* ret = attachedDevice();
    _buffer.attach(device);
    return ret;
}


void IOStream::endRead()
{
    buffer().endRead();
    if (buffer().in_avail() == 0)
        setstate(std::ios::eofbit);
}

} // namespace cxxtools


