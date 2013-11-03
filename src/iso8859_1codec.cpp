/*
 * Copyright (C) 2013 Tommi Maekitalo
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

#include "cxxtools/iso8859_1codec.h"

namespace cxxtools
{

Iso8859_1Codec::result Iso8859_1Codec::do_in(MBState& s, const char* fromBegin, const char* fromEnd, const char*& fromNext,
                                   Char* toBegin, Char* toEnd, Char*& toNext) const
{
    fromNext  = fromBegin;
    toNext = toBegin;
    while (fromNext < fromEnd && toNext < toEnd)
    {
        *toNext = Char(static_cast<unsigned char>(*fromNext));
        ++fromNext;
        ++toNext;
    }

    return ok;
}


Iso8859_1Codec::result Iso8859_1Codec::do_out(MBState& s, const Char* fromBegin, const Char* fromEnd, const Char*& fromNext,
                                                  char* toBegin, char* toEnd, char*& toNext) const
{
    fromNext  = fromBegin;
    toNext = toBegin;
    while (fromNext < fromEnd && toNext < toEnd)
    {
        *toNext = fromNext->narrow();
        ++fromNext;
        ++toNext;
    }

    return ok;
}


int Iso8859_1Codec::do_length(MBState& s, const char* fromBegin, const char* fromEnd, size_t max) const
{
    return max;
}


int Iso8859_1Codec::do_max_length() const throw()
{
    return 1;
}


bool Iso8859_1Codec::do_always_noconv() const throw()
{
    return false;
}


} // namespace cxxtools
