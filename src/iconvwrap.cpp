/*
 * Copyright (C) 2012 Jiří Pinkava - Seznam.cz a. s.
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
 */

#include "cxxtools/iconvwrap.h"

namespace cxxtools
{

iconvwrap::iconvwrap()
  : cd(iconv_t(-1))
{
}

iconvwrap::iconvwrap(const char *tocode, const char *fromcode)
{
  open(tocode, fromcode);
}

bool iconvwrap::close()
{
  if (cd != iconv_t(-1)) {
    int ret;

    ret = ::iconv_close(cd);
    cd = iconv_t(-1);

    return (ret != -1);
  }

  return true;
}

bool iconvwrap::convert(char **inbuf, size_t *inbytesleft,
    char **outbuf, size_t *outbytesleft)
{
  return (iconv(cd, inbuf, inbytesleft, outbuf, outbytesleft) != size_t(-1));
}

bool iconvwrap::is_open()
{
  return (cd != iconv_t(-1));
}

bool iconvwrap::open(const char *tocode, const char *fromcode)
{
  close();
  cd = ::iconv_open(tocode, fromcode);

  return (cd != iconv_t(-1));
}

iconvwrap::~iconvwrap()
{
  close();
}

}
