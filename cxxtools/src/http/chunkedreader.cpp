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

#include "chunkedreader.h"
#include <stdexcept>
#include <sstream>
#include <cxxtools/log.h>

log_define("cxxtools.http.chunkedreader")

namespace cxxtools
{
  namespace http
  {
    namespace
    {
      std::string charToPrint(char ch)
      {
        std::ostringstream s;
        if (ch >= 32 && ch <= 127)
          s << '<' << ch << '>';

        s << '(' << static_cast<unsigned>(static_cast<unsigned char>(ch)) << ')';
        return s.str();
      }

      void throwInvalidCharacter(char ch)
      {
        std::ostringstream s;
        s << "invalid character ";
        if (ch >= 32 && ch <= 127)
          s << '<' << ch << '>';

        s << '(' << static_cast<unsigned>(static_cast<unsigned char>(ch)) << ") in chunked encoding";

        throw std::runtime_error(s.str());
      }

    }

    ////////////////////////////////////////////////////////////////////
    // ChunkedReader

    ChunkedReader::ChunkedReader(std::streambuf* ib, unsigned bufsize)
        : _ib(ib),
          _buffer(new char[bufsize]),
          _bufsize(bufsize),
          _state(&ChunkedReader::onBegin)
    {
    }

    std::streamsize ChunkedReader::showmanyc()
    {
      log_trace("showmanyc");

      while (_state != 0
        && gptr() == egptr()
        && _ib->in_avail())
      {
        (this->*_state)();
      }

      log_debug("showmanyc=" << egptr() - gptr());
      return egptr() - gptr();
    }

    int ChunkedReader::sync()
    {
      return 0;
    }

    ChunkedReader::int_type ChunkedReader::overflow(int_type ch)
    {
      return traits_type::eof();
    }

    ChunkedReader::int_type ChunkedReader::underflow()
    {
      log_trace("ChunkedReader::underflow");

      while (_state != 0
        && gptr() == egptr()
        && _ib->sgetc() != traits_type::eof())
      {
        (this->*_state)();
      }

      if (_state == 0)
      {
        log_debug("end of chunked data reached");
        return traits_type::eof();
      }

      if (_ib->sgetc() == traits_type::eof())
      {
        log_debug("end of input stream");
        _state = 0;
        return traits_type::eof();
      }

      log_debug("not at eof - return " << charToPrint(*gptr()));
      return *gptr();
    }

    void ChunkedReader::onBegin()
    {
      char ch = _ib->sbumpc();

      log_trace("onBegin, ch=" << charToPrint(ch));

      if (ch >= '0' && ch <= '9')
      {
        _chunkSize = ch - '0';
        _state = &ChunkedReader::onSize;
      }
      else if (ch >= 'a' && ch <= 'f')
      {
        _chunkSize = ch - 'a' + 10;
        _state = &ChunkedReader::onSize;
      }
      else if (ch >= 'A' && ch <= 'F')
      {
        _chunkSize = ch - 'A' + 10;
        _state = &ChunkedReader::onSize;
      }
      else
        throwInvalidCharacter(ch);
    }

    void ChunkedReader::onSize()
    {
      char ch = _ib->sbumpc();

      log_trace("onSize, ch=" << charToPrint(ch));

      if (ch >= '0' && ch <= '9')
      {
        _chunkSize = _chunkSize * 16 + (ch - '0');
      }
      else if (ch >= 'a' && ch <= 'f')
      {
        _chunkSize = _chunkSize * 16 + (ch - 'a' + 10);
      }
      else if (ch >= 'A' && ch <= 'F')
      {
        _chunkSize = _chunkSize * 16 + (ch - 'A' + 10);
      }
      else
      {
        log_debug("chunk size=" << _chunkSize);

        if (ch == '\r')
        {
          _state = &ChunkedReader::onEndl;
        }
        else if (ch == '\n')
        {
          if (_chunkSize > 0)
            _state = &ChunkedReader::onData;
          else
            _state = 0;
        }
        else
        {
          _state = &ChunkedReader::onExtension;
        }
      }
    }

    void ChunkedReader::onEndl()
    {
      char ch = _ib->sbumpc();

      log_trace("onEndl, ch=" << charToPrint(ch));

      if (ch == '\n')
      {
        if (_chunkSize > 0)
          _state = &ChunkedReader::onData;
        else
          _state = 0;
      }
      else
        throwInvalidCharacter(ch);
    }

    void ChunkedReader::onExtension()
    {
      log_trace("onExtension");

      char ch = _ib->sbumpc();

      if (ch == '\r')
      {
        _state = &ChunkedReader::onEndl;
      }
      else if (ch == '\n')
      {
        if (_chunkSize > 0)
          _state = &ChunkedReader::onData;
        else
          _state = 0;
      }
    }

    void ChunkedReader::onData()
    {
      log_trace("onData");

      std::streamsize n = _ib->in_avail();

      if (n > _bufsize)
        n = _bufsize;

      if (n > _chunkSize)
        n = _chunkSize;

      n = _ib->sgetn(_buffer, n);
      setg(_buffer, _buffer, _buffer + n);

      _chunkSize -= n;

      if (_chunkSize <= 0)
        _state = &ChunkedReader::onDataEnd0;

    }

    void ChunkedReader::onDataEnd0()
    {
      char ch = _ib->sbumpc();

      log_trace("onDataEnd0, ch=" << charToPrint(ch));

      if (ch == '\r')
      {
        log_debug("=> onDataEnd");
        _state = &ChunkedReader::onDataEnd;
      }
      else if (ch == '\n')
      {
        log_debug("=> onBegin");
        _state = &ChunkedReader::onBegin;
      }
      else
        throwInvalidCharacter(ch);
    }

    void ChunkedReader::onDataEnd()
    {
      char ch = _ib->sbumpc();

      log_trace("onDataEnd, ch=" << charToPrint(ch));

      if (ch == '\n')
      {
        log_debug("=> onBegin");
        _state = &ChunkedReader::onBegin;
      }
      else
        throwInvalidCharacter(ch);
    }

  }
}
