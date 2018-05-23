/*
 * Copyright (C) 2018 Tommi Maekitalo
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

#ifndef CXXTOOLS_PTRSTREAM_H
#define CXXTOOLS_PTRSTREAM_H

#include <cxxtools/ptrstreambuf.h>
#include <iostream>

namespace cxxtools
{

/** The class implements a iostream interface to a char buffer.
 *
 *  The user is responsible to provide a buffer, which can be read and written to.
 *  The buffer must be valid as long as the class reads from or writes to it.
 */
template <typename CharT>
class BasicPtrStream : public std::basic_iostream<CharT>
{
    BasicPtrStreamBuf<CharT> _streambuf;

public:
    BasicPtrStream(CharT* begin, CharT* end)
        : _streambuf(begin, end)
    {
        std::basic_iostream<CharT>::init(&_streambuf);
    }

    BasicPtrStream(CharT* begin, size_t size)
        : _streambuf(begin, size)
    {
        std::basic_iostream<CharT>::init(&_streambuf);
    }

    CharT* begin() const  { return _streambuf.begin(); }
    CharT* end() const    { return _streambuf.end(); }
};

typedef BasicPtrStream<char> PtrStream;

}

#endif
