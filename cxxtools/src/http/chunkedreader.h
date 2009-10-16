/*
 * Copyright (C) 2009 Tommi Maekitalo
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

#ifndef HTTP_CHUNKEDHREADER_H
#define HTTP_CHUNKEDHREADER_H

#include <streambuf>
#include <iostream>

namespace cxxtools
{
  namespace http
  {
    class ChunkedReader : public std::streambuf
    {
        std::streambuf* _ib;
        char* _buffer;
        unsigned _bufsize;
        unsigned _chunkSize;

        void (ChunkedReader::*_state)();

        void onBegin();
        void onSize();
        void onEndl();
        void onExtension();
        void onData();
        void onDataEnd0();
        void onDataEnd();

      public:
        explicit ChunkedReader(std::streambuf* ib, unsigned bufsize = 8192);

        void reset()      { _state = &ChunkedReader::onBegin; setg(0, 0, 0); }
        bool eod() const  { return _state == 0; }

        std::streamsize showmanyc();
        virtual int sync();
        virtual int_type overflow(int_type ch);
        virtual int_type underflow();

    };

    class ChunkedIStream : public std::istream
    {
        ChunkedReader _streambuf;

      public:
        explicit ChunkedIStream(std::streambuf* ib)
          : std::istream(&_streambuf),
            _streambuf(ib)
          { }

        void reset()        { _streambuf.reset(); clear(); }
        bool eod() const    { return _streambuf.eod(); }
    };

  }
}

#endif // HTTP_CHUNKEDHREADER_H
