/*
 * Copyright (C) 2017 Tommi Maekitalo
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

#include <cxxtools/stddevice.h>
#include <cxxtools/ioerror.h>
#include "iodeviceimpl.h"

#include <unistd.h>

namespace cxxtools
{

StdinDevice::StdinDevice()
{
    _impl = new IODeviceImpl(*this);
    setEnabled(true);
    setAsync(true);
    setEof(false);
    _impl->open(STDIN_FILENO, true, true);
}

StdinDevice::~StdinDevice()
{
    _impl->detach(*selector());
    setEnabled(false);
    delete _impl;
}

IODeviceImpl& StdinDevice::ioimpl()
{
    return *_impl;
}

SelectableImpl& StdinDevice::simpl()
{
    return *_impl;
}

size_t StdinDevice::onBeginWrite(const char* buffer, size_t n)
{
    throw IOError("cannot write to standard input device");
}

size_t StdinDevice::onWrite(const char* buffer, size_t count)
{
    throw IOError("cannot write to standard input device");
}

ODevice::ODevice()
{
    _impl = new IODeviceImpl(*this);
    setEnabled(true);
    setAsync(true);
    setEof(false);
}

ODevice::~ODevice()
{
    _impl->detach(*selector());
    setEnabled(false);
    delete _impl;
}

IODeviceImpl& ODevice::ioimpl()
{
    return *_impl;
}

SelectableImpl& ODevice::simpl()
{
    return *_impl;
}

size_t ODevice::onBeginRead(char* buffer, size_t n, bool& eof)
{
    throw IOError("cannot read from standard output device");
}

size_t ODevice::onRead(char* buffer, size_t count, bool& eof)
{
    throw IOError("cannot read from standard output device");
}

StdoutDevice::StdoutDevice()
{
    ioimpl().open(STDOUT_FILENO, true, true);
}

StderrDevice::StderrDevice()
{
    ioimpl().open(STDERR_FILENO, true, true);
}

} // namespace cxxtools
