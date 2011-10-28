/*
 * Copyright (C) 2006 Tommi Maekitalo
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

#include <cxxtools/fdstream.h>
#include <cxxtools/systemerror.h>
#include <cxxtools/log.h>
#include <algorithm>
#include <unistd.h>
#include <errno.h>

log_define("cxxtools.fdstream")

namespace cxxtools
{
  Fdstreambuf::Fdstreambuf(int fd_, unsigned bufsize_, bool doClose_)
    : fd(fd_),
      doClose(doClose_),
      bufsize(bufsize_),
      buffer(0)
  { }

  Fdstreambuf::~Fdstreambuf()
  {
    delete [] buffer;

    if (doClose)
      ::close(fd);
  }

  void Fdstreambuf::close()
  {
    ::close(fd);
    doClose = false;
  }

  std::streambuf::int_type Fdstreambuf::overflow(std::streambuf::int_type ch)
  {
    log_debug("overflow(" << ch << ')');

    setg(0, 0, 0);

    if (pptr() > buffer)
    {
      log_debug("write " << (pptr() - buffer) << " bytes to fd " << fd);
      ssize_t ret;
      do {
        ret = ::write(fd, buffer, pptr() - buffer);
      } while (ret == -1 && errno == EINTR);

      if(ret < 0)
        throw SystemError(errno, "write");

      if (ret == 0)
        return traits_type::eof();

      log_debug(ret << " bytes written to fd " << fd);
      ssize_t rest = pptr() - buffer - ret;
      std::copy(buffer + ret, buffer + ret + rest, buffer);
      setp(buffer + rest, buffer + bufsize);
    }
    else
    {
      log_debug("initialize outputbuffer");
      if (buffer == 0)
      {
        log_debug("allocate " << bufsize << " bytes output buffer");
        buffer = new char[bufsize];
      }

      setp(buffer, buffer + bufsize);
    }

    if (ch != traits_type::eof())
    {
      *pptr() = traits_type::to_char_type(ch);
      pbump(1);
    }

    return 0;
  }

  std::streambuf::int_type Fdstreambuf::underflow()
  {
    if (sync() != 0)
      return traits_type::eof();

    if (buffer == 0)
    {
      log_debug("allocate " << bufsize << " bytes input buffer");
      buffer = new char[bufsize];
    }

    log_debug("read from fd " << fd);
    ssize_t ret;
    do {
      ret = ::read(fd, buffer, bufsize);
    } while (ret == -1 && errno == EINTR);

    if(ret < 0)
      throw SystemError(errno, "read");

    if (ret == 0)
      return traits_type::eof();

    log_debug(ret << " bytes read");
    setg(buffer, buffer, buffer + ret);

    return traits_type::to_int_type(*gptr());
  }

  int Fdstreambuf::sync()
  {
    log_debug("sync()");
    if (pptr() != pbase())
    {
      char* p = pbase();
      while (p < pptr())
      {
        log_debug("write " << (pptr() - p) << " bytes to fd " << fd);
        ssize_t ret = ::write(fd, p, pptr() - p);

        if(ret < 0)
          throw SystemError(errno, "write");

        if (ret == 0)
          return traits_type::eof();

        log_debug(ret << " bytes written to fd " << fd);
        p += ret;
      }
    }

    setp(0, 0);
    setg(0, 0, 0);

    return 0;
  }

}
