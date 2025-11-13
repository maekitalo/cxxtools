/*
 * Copyright (C) 2025 Tommi Mäkitalo
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
#ifndef CXXTOOLS_BUFFEREDREADER_H
#define CXXTOOLS_BUFFEREDREADER_H

#include <cxxtools/iodevice.h>
#include <vector>

namespace cxxtools
{
class IODevice;

/** @brief reads data buffered from a cxxtools::IODevice
 *
 *  BufferedReader maintains a input buffer to optimize reading data
 *  from a device.
 */
class BufferedReader
{
    cxxtools::IODevice& _device;
    unsigned _bufsize;
    std::vector<char> _inputBuffer;
    unsigned _getp;

    void uflow();

public:
    BufferedReader(cxxtools::IODevice& device, unsigned bufsize = 8192)
        : _device(device),
          _bufsize(bufsize),
          _getp(0)
        { }

    //! returns the number of bytes currently cached in the input buffer
    unsigned in_avail() const   { return _inputBuffer.size() - _getp; }
    //! returns the pointer to the position of the next byte in the buffer
    const char* gptr() const    { return _inputBuffer.data() + _getp; }
    //! returns the pointer to the end of input buffer
    const char* egptr() const   { return _inputBuffer.data() + _inputBuffer.size(); }

    /** @brief reads data from a device using the input buffer

        If there no data available, 0 is returnd and the eof is set.
     */
    unsigned read(char* data, unsigned count);

    /** @brief returns the next available byte

        If the input buffer is empty, it is filled from the input device.
        If the eof is reached, '\0' is returned and eof flag is set.
     */
    char peek();

    /** @brief returns the next available byte and removes it from the input buffer

        If the input buffer is empty, it is filled from the input device.
        If the eof is reached, '\0' is returned and eof flag is set.
     */
    char get();

    //! empties the input buffer
    void discard()              { _inputBuffer.clear(); _getp = 0; }
    //! returns true, if the end of the input device was reached
    bool eof() const            { return _device.eof(); }
};

}

#endif
