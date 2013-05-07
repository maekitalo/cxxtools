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

#ifndef CXXTOOLS_ICONVWRAP_H
#define CXXTOOLS_ICONVWRAP_H

#include <iconv.h>

namespace cxxtools
{

  /** Wraps iconv. */
class iconvwrap {
  public:

    /** Create iconvwrap object. */
    iconvwrap();

    /** Create iconvwrap object and initializes it.
     *
     * @param tocode destination encoding name
     * @param fromcode source encoding name
     */
    iconvwrap(const char *tocode, const char *fromcode);

    /** Close iconvwrap object, release resouces.
     *
     * @return true on succes, otherwise return false and set errno. */
    bool close();

    /** Recode input string into output buffer.
     *
     * @param inbuf
     * @param inbytesleft
     * @param outbuf
     * @param outbytesleft
     * @return true if all succesfully converted, on error returns false
     *      and se errno
     */
    bool convert(char **inbuf, size_t *inbytesleft,
        char **outbuf, size_t *outbytesleft);


    /** Return true if IConv is open, false otherwise. */
    bool is_open();

    /** (Re)initializes iconvwrap object.
     *
     * @param tocode target encoding name
     * @param fromcode source encoding name
     * @return true on succes, on error return false and set errno
     */
    bool open(const char *tocode, const char *fromcode);

    /** Destroy iconvwrap object. */
    ~iconvwrap();

  protected:
    iconv_t cd;
};

}

#endif
