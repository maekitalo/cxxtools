/* iconvstream.cpp
 *
 * cxxtools - general purpose C++-toolbox
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
 *
 */

#include "cxxtools/iconvstream.h"
#include "cxxtools/log.h"
#include <string.h>
#include "config.h"
#include <errno.h>
#include <stdexcept>

log_define("cxxtools.iconvstream")

namespace cxxtools
{

iconvstreambuf* iconvstreambuf::open(std::ostream& sink_,
  const char* tocode, const char* fromcode)
{
  log_debug("iconv_open(\"" << tocode << "\", \"" << fromcode << "\")");
  sink = &sink_;
  cd = iconv_open(tocode, fromcode);

  if (cd == (iconv_t)-1)
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

  log_debug("iconv-handle=" << cd);

  setp(buffer, buffer + (sizeof(buffer) - 1));
  return this;
}

iconvstreambuf* iconvstreambuf::close() throw()
{
  if (cd != (iconv_t)-1)
  {
    sync();
    log_debug("iconv_close(" << cd << ')');
    int r = iconv_close(cd);
    if (r == 0)
    {
      cd = (iconv_t)-1;
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
    log_debug("iconv(" << cd << ") " << inbytesleft << " bytes");
    size_t s = iconv(cd,
          &inbufptr, &inbytesleft,
          &outbufptr, &outbytesleft);

    if (s < 0 && errno != 0 && errno != EINVAL && errno != E2BIG)
    {
      log_warn("convert failed");
      return traits_type::eof();
    }

    log_debug("pass " << outbufptr - outbuf << " bytes to sink");
    sink->write(outbuf, outbufptr - outbuf);

    if (sink->fail())
    {
      log_warn("sink failed");
      return traits_type::eof();
    }

    log_debug("reinitialize put area");
    setp(buffer, buffer + (sizeof(buffer) - 1));

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

void iconvstream::open(std::ostream& sink_,
  const char* tocode, const char* fromcode)
{
  if (!fail() && streambuf.open(sink_, tocode, fromcode) == 0)
    setstate(std::ios::failbit);
}

}
