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

#ifndef CXXTOOLS_STDDEVICE_H
#define CXXTOOLS_STDDEVICE_H

#include <cxxtools/iodevice.h>

namespace cxxtools
{

class StdinDevice : public IODevice
{
    public:
        StdinDevice();

        ~StdinDevice();

    protected:
        IODeviceImpl& ioimpl();

        SelectableImpl& simpl();

        size_t onBeginWrite(const char* buffer, size_t n);

        size_t onWrite(const char* buffer, size_t count);

    private:
        IODeviceImpl* _impl;
};

class ODevice : public IODevice
{
    protected:
        ODevice();

        ~ODevice();

        IODeviceImpl& ioimpl();

        SelectableImpl& simpl();

        size_t onBeginRead(char* buffer, size_t n, bool& eof);

        size_t onRead(char* buffer, size_t count, bool& eof);

    private:
        IODeviceImpl* _impl;
};

class StdoutDevice : public ODevice
{
    public:
        StdoutDevice();
};

class StderrDevice : public ODevice
{
    public:
        StderrDevice();
};

}

#endif
