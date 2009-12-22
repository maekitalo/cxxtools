/*
 * Copyright (C) 2007 Tommi Maekitalo
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

#include <cxxtools/posix/pipestream.h>
#include <cxxtools/syserror.h>
#include <algorithm>
#include <unistd.h>
#include <cxxtools/log.h>
#include <cstring>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

log_define("cxxtools.pipestream")

namespace cxxtools
{
namespace posix
{
  Pipestreambuf::Pipestreambuf(unsigned bufsize_)
    : bufsize(bufsize_),
      ibuffer(0),
      obuffer(0)
  { }

  Pipestreambuf::~Pipestreambuf()
  {
    log_debug("Pipestreambuf::~Pipestreambuf()");

    try
    {
      closeReadFd();
    }
    catch (const std::exception& e)
    {
      log_debug("ignore exception in closing read pipe: " << e.what());
    }

    try
    {
      closeWriteFd();
    }
    catch (const std::exception& e)
    {
      log_debug("ignore exception in closing write pipe: " << e.what());
    }

    delete [] ibuffer;
    delete [] obuffer;
  }

  std::streambuf::int_type Pipestreambuf::overflow(std::streambuf::int_type ch)
  {
    log_debug("overflow(" << ch << ')');

    if (pptr() != pbase())
    {
      log_debug("write " << (pptr() - pbase()) << " bytes to fd " << getWriteFd());
      ssize_t ret = ::write(getWriteFd(), pbase(), pptr() - pbase());

      if(ret < 0)
        throw SystemError(errno, "write");

      if (ret == 0)
        return traits_type::eof();
      else
      {
        log_debug(ret << " bytes written to fd " << getWriteFd());
        if (ret < bufsize)
          std::memmove(obuffer, obuffer + ret, bufsize - ret);
        setp(obuffer, obuffer + bufsize);
        pbump(bufsize - ret);
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

  std::streambuf::int_type Pipestreambuf::underflow()
  {
    log_debug("underflow()");

    if (ibuffer == 0)
    {
      log_debug("allocate " << bufsize << " bytes input buffer");
      ibuffer = new char[bufsize];
    }

    log_debug("read from fd " << getReadFd());
    int ret = ::read(getReadFd(), ibuffer, bufsize);
    log_debug("read returned " << ret);

    if(ret < 0)
      throw SystemError(errno, "read");

    if (ret == 0)
      return traits_type::eof();

    log_debug(ret << " bytes read");
    setg(ibuffer, ibuffer, ibuffer + ret);

    return *gptr();
  }

  int Pipestreambuf::sync()
  {
    log_debug("sync()");
    if (pptr() != pbase())
    {
      char* p = pbase();
      while (p < pptr())
      {
        log_debug("write " << (pptr() - p) << " bytes to fd " << getWriteFd());
        ssize_t ret = ::write(getWriteFd(), p, pptr() - p);

        if(ret < 0)
          throw SystemError(errno, "write");

        if (ret == 0)
          return traits_type::eof();

        log_debug(ret << " bytes written to fd " << getWriteFd());
        p += ret;
      }
      setp(obuffer, obuffer + bufsize);
    }
    return 0;
  }

}
}
