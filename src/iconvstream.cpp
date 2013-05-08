/*
 * Copyright (C) 2003 Tommi Maekitalo
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

#include "cxxtools/iconvstream.h"
#include "cxxtools/log.h"
#include <string.h>
#include "config.h"
#include <errno.h>
#include <stdexcept>
#include <sstream>

log_define("cxxtools.iconvstream")

namespace cxxtools
{

iconv_error::iconv_error(size_t pos)
  : std::runtime_error(std::string()), pos(pos)
{
}

size_t iconv_error::position() const throw()
{
  return pos;
}

const char *iconv_error::what() const throw()
{
  if (msg.empty())
  {
    std::ostringstream ss;

    ss << "iconv failed to convert character at position: " << pos;
    msg = ss.str();
  }

  return msg.c_str();
}

iconv_error::~iconv_error() throw()
{
}

iconvstreambuf* iconvstreambuf::open(std::ostream& sink_,
  const char* tocode, const char* fromcode)
{
  open(sink_, tocode, fromcode, iconvstreambuf::mode_default);
}

iconvstreambuf* iconvstreambuf::open(std::ostream& sink_,
  const char* tocode, const char* fromcode, mode_t mode_)
{
  log_debug("iconv_open(\"" << tocode << "\", \"" << fromcode <<
          "\", " << mode_ << ")");
  mode = mode_;
  sink = &sink_;
  pos = 0;

  if (!conv.open(tocode, fromcode))
  {
    if (errno == EINVAL)
    {
      std::string msg = "conversion not supported; from=\"";
      msg += fromcode;
      msg += "\" to \"";
      msg += tocode;
      log_warn(msg);
      throw std::runtime_error(msg);
    }
    return 0;
  }

  log_debug("iconv-opened");

  return this;
}

iconvstreambuf* iconvstreambuf::close() throw()
{
  if (conv.is_open())
  {
    sync();
    log_debug("iconv.close(...)");
    if (conv.close())
    {
      return this;
    }
    else
      return 0;
  }
  else
    return this;
}

iconvstreambuf::int_type iconvstreambuf::overflow(int_type c)
{
  log_debug("overflow(" << c << ')');

  if (sink == 0)
  {
    log_error("no sink");
    return traits_type::eof();
  }
  else if (pptr() == 0 || pptr() == buffer)
  {
    log_debug("empty put-area");
    if (c != traits_type::eof())
    {
      if (pptr() == 0)
      {
        log_debug("initialize buffer");
        setp(buffer, buffer + (sizeof(buffer) - 1));
      }
      *pptr() = (char_type)c;
      pbump(1);
    }
    return 0;
  }
  else
  {
    char outbuf[sizeof(buffer)*2];;

    size_t inbytesleft = pptr() - buffer;
    if (c != traits_type::eof())
    {
      *pptr() = (char_type)c;
      ++inbytesleft;
    }
    size_t outbytesleft = sizeof(outbuf);

    ICONV_CONST char* inbufptr = buffer;
    char* outbufptr = outbuf;

    // convert as many charachters as possible
    log_debug("iconv(...) " << inbytesleft << " bytes");
    bool iconv_ret = conv.convert(&inbufptr, &inbytesleft,
          &outbufptr, &outbytesleft);

    pos += inbufptr - buffer;
    iconv_ret = iconv_ret || errno == EINVAL || errno == E2BIG;

    log_debug("pass " << outbufptr - outbuf << " bytes to sink");
    sink->write(outbuf, outbufptr - outbuf);

    if (sink->fail())
    {
      log_warn("sink failed");
      return traits_type::eof();
    }

    log_debug("reinitialize put area");
    setp(buffer, buffer + (sizeof(buffer) - 1));

    if (!iconv_ret) {
      switch (mode) {
        case iconvstreambuf::mode_skip:
          log_warn("convert failed, skipping byte");
          --inbytesleft;
          ++inbufptr;
          iconv_ret = true;
        break;
        case iconvstreambuf::mode_throw:
          log_warn("convert failed, throwing exception");
          throw iconv_error(pos);
        default:
          return traits_type::eof();
      }
    }

    // move left bytes to the start of buffer and reinitialize buffer
    if (inbytesleft > 0)
    {
      log_debug("move " << inbytesleft << " bytes to the start");
      sputn(inbufptr, inbytesleft);
    }

    return 0;
  }
}

iconvstream::int_type iconvstreambuf::underflow()
{
  log_warn("underflow not supported in iconvstreambuf");
  return traits_type::eof();
}

int iconvstreambuf::sync()
{
  log_debug("sync");

  if (pptr() == 0 || pptr() == buffer)
    return 0;
  else if (sink)
  {
    size_t ob, ob2;
    int_type ret;
    do
    {
      ob = pptr() - buffer;
      ret = overflow(traits_type::eof());
      ob2 = pptr() - buffer;
    } while (ob2 > 0 && ob2 < ob);

    log_debug("flush sink");
    sink->flush();

    return ret == traits_type::eof() || sink->fail() ? -1 : 0;
  }
  else
  {
    log_warn("no sink");
    return 0;
  }
}

std::streampos iconvstreambuf::seekoff(std::streamoff off,
  std::ios_base::seekdir way, std::ios_base::openmode which)
{
  // modify off to by relative position from current position
  switch (way) {
    case std::ios_base::beg:
      off -= pos;
    case std::ios_base::cur:
      break;
    default:
      return std::streampos(-1);
  }

  if (which == std::ios_base::in) {
    char *buf_p = gptr() + off;
    // alow seek only on valid data
    if (buf_p < eback() || buf_p > pptr())
        return std::streampos(-1);

    gbump(off);
    pos += off;
    return pos;
  }

  if (which == std::ios_base::out) {
    char *buf_p = pptr() + off;
    // alow seek only on valid data
    if (buf_p < pbase() || buf_p > pptr())
        return std::streampos(-1);

    pbump(off);
    return pos + off;
  }

  return std::streampos(-1);
}

std::streampos iconvstreambuf::seekpos(std::streampos sp,
  std::ios_base::openmode which)
{
  return seekoff(sp, std::ios_base::beg, which);
}

void iconvstream::open(std::ostream& sink_,
  const char* tocode, const char* fromcode)
{
  open(sink_, tocode, fromcode, iconvstreambuf::mode_default);
}

void iconvstream::open(std::ostream& sink_,
  const char* tocode, const char* fromcode,
  iconvstreambuf::mode_t mode)
{
  if (!fail() && streambuf.open(sink_, tocode, fromcode, mode) == 0)
    setstate(std::ios::failbit);
}

}
