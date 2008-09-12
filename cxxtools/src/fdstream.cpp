/*
 * Copyright (C) 2006 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
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
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

#include <cxxtools/fdstream.h>
#include <cxxtools/syserror.h>
#include <algorithm>
#include <unistd.h>
#include <cxxtools/log.h>

log_define("cxxtools.fdstream")

namespace cxxtools
{
  Fdstreambuf::Fdstreambuf(int fd_, unsigned bufsize_, bool doClose_)
    : fd(fd_),
      doClose(doClose_),
      bufsize(bufsize_),
      ibuffer(0),
      obuffer(0)
  { }

  Fdstreambuf::~Fdstreambuf()
  {
    delete [] ibuffer;
    delete [] obuffer;

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

    if (pptr() != pbase())
    {
      log_debug("write " << (pptr() - pbase()) << " bytes to fd " << fd);
      ssize_t ret = ::write(fd, pbase(), pptr() - pbase());
      if (ret < 0)
        throw SysError("write");
      else if (ret == 0)
        return traits_type::eof();
      else
      {
        log_debug(ret << " bytes written to fd " << fd);
        std::copy(pptr(), pptr() + ret, obuffer);
        setp(obuffer + ret, obuffer + bufsize - ret);
      }
    }
    else
    {
      log_debug("initialize outputbuffer");
      if (obuffer == 0)
      {
        log_debug("allocate " << bufsize << " bytes output buffer");
        obuffer = new char[bufsize];
      }

      setp(obuffer, obuffer + bufsize);
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

    if (ibuffer == 0)
    {
      log_debug("allocate " << bufsize << " bytes input buffer");
      ibuffer = new char[bufsize];
    }

    log_debug("read from fd " << fd);
    int ret = ::read(fd, ibuffer, bufsize);
    if (ret < 0)
      throw SysError("read");
    else if (ret == 0)
      return traits_type::eof();

    log_debug(ret << " bytes read");
    setg(ibuffer, ibuffer, ibuffer + ret);

    return traits_type::to_char_type(*gptr());
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
        if (ret < 0)
          throw SysError("write");
        else if (ret == 0)
          return traits_type::eof();

        log_debug(ret << " bytes written to fd " << fd);
        p += ret;
      }
    }
    return 0;
  }

}
