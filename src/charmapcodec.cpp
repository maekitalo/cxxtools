/*
 * Copyright (C) 2015 Tommi Maekitalo
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

#include "cxxtools/charmapcodec.h"
#include "cxxtools/log.h"

log_define("cxxtools.charmapcodec")

namespace cxxtools
{

namespace
{
    typedef Char::value_type CharMap[256];

    inline Char toUChar(const CharMap& charMap, char ch)
    {
        return Char(charMap[static_cast<unsigned char>(ch)]);
    }

    char toChar(const CharMap& charMap, Char ch)
    {
        // for optimization look up identity mapping first
        if (ch.value() < 256 && charMap[ch.value()] == ch.value())
            return static_cast<char>(ch.value());

        for (unsigned n = 0; n < 256; ++n)
        {
            if (charMap[n] == ch.value())
                return static_cast<char>(n);
        }
        return ' ';
    }
}

CharMapCodec::result CharMapCodec::do_in(MBState& /*s*/, const char* fromBegin, const char* fromEnd, const char*& fromNext,
                                   Char* toBegin, Char* toEnd, Char*& toNext) const
{
    fromNext  = fromBegin;
    toNext = toBegin;
    while (fromNext < fromEnd && toNext < toEnd)
    {
        *toNext = toUChar(_charMap, *fromNext);
        ++fromNext;
        ++toNext;
    }

    return ok;
}


CharMapCodec::result CharMapCodec::do_out(MBState& /*s*/, const Char* fromBegin, const Char* fromEnd, const Char*& fromNext,
                                                  char* toBegin, char* toEnd, char*& toNext) const
{
    fromNext  = fromBegin;
    toNext = toBegin;
    while (fromNext < fromEnd && toNext < toEnd)
    {
        *toNext = toChar(_charMap, *fromNext);
        log_debug(static_cast<unsigned>(fromNext->value()) << " => " << static_cast<unsigned>(static_cast<unsigned char>(*toNext)));
        ++fromNext;
        ++toNext;
    }

    return ok;
}


int CharMapCodec::do_length(MBState& /*s*/, const char* /*fromBegin*/, const char* /*fromEnd*/, size_t max) const
{
    return max;
}


int CharMapCodec::do_max_length() const throw()
{
    return 1;
}


bool CharMapCodec::do_always_noconv() const throw()
{
    return false;
}


} // namespace cxxtools
