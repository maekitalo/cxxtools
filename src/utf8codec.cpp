/*
 * Copyright (C) 2004 Marc Boris Duerner
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
#include "cxxtools/utf8codec.h"
#include <cstring>

#define byteMask 0xBF
#define byteMark 0x80

namespace cxxtools {

const Char ReplacementChar = Char(0x0000FFFD);
const Char MaxBmp = Char(0x0000FFFF);
const Char MaxUtf16 = Char(0x0010FFFF);
const Char MaxUtf32 = Char(0x7FFFFFFF);
const Char MaxLegalUtf32 = Char(0x0010FFFF);
const Char SurHighStart = Char(0xD800);
const Char SurHighEnd = Char(0xDBFF);
const Char SurLowStart = Char(0xDC00);
const Char SurLowEnd = Char(0xDFFF);
const Char ByteOrderMark = Char(0xFEFF);
const Char ByteOrderSwapped = Char(0xFFFE);

/*
 * Index into the table below with the first byte of a UTF-8 sequence to
 * get the number of trailing bytes that are supposed to follow it.
 * Note that *legal* UTF-8 values can't have 4 or 5-bytes. The table is
 * left as-is for anyone who may want to do such conversion, which was
 * allowed in earlier algorithms.
 */
const char trailingBytesForUTF8[256] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5
};


/*
 * Magic values subtracted from a buffer value during UTF8 conversion.
 * This table contains as many values as there might be trailing bytes
 * in a UTF-8 sequence.
 */
const Char::value_type offsetsFromUTF8[6] = {
    Char::value_type(0x00000000),
    Char::value_type(0x00003080),
    Char::value_type(0x000E2080),
    Char::value_type(0x03C82080),
    Char::value_type(0xFA082080),
    Char::value_type(0x82082080)
};


/*
 * Once the bits are split out into bytes of UTF-8, this is a mask OR-ed
 * into the first byte, depending on how many bytes follow.  There are
 * as many entries in this table as there are UTF-8 sequence types.
 * (I.e., one byte sequence, two byte... etc.). Remember that sequencs
 * for *legal* UTF-8 will be 4 or fewer bytes total.
 */
const uint8_t firstByteMark[7] = {
    0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC
};


/**
 * @brief Checks if the given character sequence is a valid UTF-8 character.
 *
 * The given array 8-bit-values is "parsed" and tried to be converted into a
 * Unicode-character using UTF-decoding. If this is not possible $false$ is returned
 * as the 8-bit-sequence is not a valid UTF-8 character. Otherwise $true$ is
 * returned. Only the first number of characters as specified in 'length' is
 * tried to converted.
 *
 * @param source An array of 8-bit values containing raw UTF-8 character data.
 * @param length Number of characters of source which are checked if they are
 * a valid UTF-8 character.
 * @return $true$ if the given sequence is a UTF-8-encoded character, $false$
 * otherwise.
 */
inline bool isLegalUTF8(const uint8_t *source, int length)
{
    uint8_t a;
    const uint8_t *srcptr = source + length;

    switch (length)
    {
        default:
            return false;

        // Everything else falls through when "true"...
        case 4: if ((a = (*--srcptr)) < 0x80 || a > 0xBF) return false;
        case 3: if ((a = (*--srcptr)) < 0x80 || a > 0xBF) return false;
        case 2: if ((a = (*--srcptr)) > 0xBF) return false;
            switch (*source) {
                // no fall-through in this inner switch
                case 0xE0: if (a < 0xA0) return false; break;
                case 0xED: if (a > 0x9F) return false; break;
                case 0xF0: if (a < 0x90) return false; break;
                case 0xF4: if (a > 0x8F) return false; break;
                default:   if (a < 0x80) return false;
            }
        case 1: if (*source >= 0x80 && *source < 0xC2) return false;
    }

    if (*source > 0xF4)
        return false;

    return true;
}


Utf8Codec::Utf8Codec(size_t ref)
: TextCodec<Char, char>(ref)
{}


namespace
{
    inline unsigned short numBytes(const MBState& s, const char* fromBegin, const char* fromEnd)
    {
        return fromEnd - fromBegin + s.n;
    }

    inline char getByte(const MBState& s, const char* fromBegin, const char* fromEnd, unsigned short n)
    {
        if (n < s.n)
            return s.value.mbytes[n];
        else
            return fromBegin[n - s.n];
    }
}


Utf8Codec::result Utf8Codec::do_in(MBState& s, const char* fromBegin, const char* fromEnd, const char*& fromNext,
                                   Char* toBegin, Char* toEnd, Char*& toNext) const
{
    Utf8Codec::result retstat = ok;
    fromNext = fromBegin;
    toNext = toBegin;

    // check for empty input
    if (fromEnd == fromBegin)
        return ok;

    // check for incomplete byte order mark:
    if (numBytes(s, fromBegin, fromEnd) < 3)
    {
        if (getByte(s, fromBegin, fromEnd, 0) == '\xef')
        {
            while (fromNext < fromEnd)
                s.value.mbytes[s.n++] = *fromNext++;
            return ok;
        }
    }
    else
    {
        // skip byte order mark
        if (getByte(s, fromBegin, fromEnd, 0) == '\xef'
            && getByte(s, fromBegin, fromEnd, 1) == '\xbb'
            && getByte(s, fromBegin, fromEnd, 2) == '\xbf')
        {
            if (s.n <= 3)
            {
                fromNext += 3 - s.n;
                s.n = 0;
            }
            else
            {
                std::memmove(s.value.mbytes, s.value.mbytes + 3, s.n - 3);
                s.n -= 3;
            }
        }
    }

    while (fromNext < fromEnd)
    {
        if (toNext >= toEnd)
        {
            retstat = partial;
            break;
        }

        if (s.n < sizeof(s.value.mbytes))
        {
            s.value.mbytes[s.n++] = *fromNext++;
        }

        uint8_t* fnext = reinterpret_cast<uint8_t *>(&s.value.mbytes[0]);
        uint8_t* fend = fnext + s.n;

        const size_t extraBytesToRead = trailingBytesForUTF8[*fnext];
        if (fnext + extraBytesToRead >= fend)
        {
            retstat = partial;
            break;
        }

        if( !isLegalUTF8( fnext, extraBytesToRead + 1 ) )
        {
            retstat = error;
            break;
        }

        *toNext = Char(0);
        switch (extraBytesToRead)
        {
            case 5: *toNext = Char((toNext->value() + *fnext++) << 6); // We should never get this for legal UTF-8
            case 4: *toNext = Char((toNext->value() + *fnext++) << 6); // We should never get this for legal UTF-8
            case 3: *toNext = Char((toNext->value() + *fnext++) << 6);
            case 2: *toNext = Char((toNext->value() + *fnext++) << 6);
            case 1: *toNext = Char((toNext->value() + *fnext++) << 6);
            case 0: *toNext = Char((toNext->value() + *fnext++));
        }

        *toNext = Char(toNext->value() - offsetsFromUTF8[extraBytesToRead]);

        // UTF-16 surrogate values are illegal in UTF-32, and anything
        // over Plane 17 (> 0x10FFFF) is illegal.
        if (*toNext > MaxLegalUtf32)
        {
            *toNext = ReplacementChar;
        }
        else if(*toNext >= SurHighStart && *toNext <= SurLowEnd)
        {
            *toNext = ReplacementChar;
        }

        s.n = 0;
        ++toNext;
    }

    return retstat;
}


Utf8Codec::result Utf8Codec::do_out(MBState& /*s*/, const Char* fromBegin, const Char* fromEnd, const Char*& fromNext,
                                                  char* toBegin, char* toEnd, char*& toNext) const
{
    result retstat = ok;
    fromNext  = fromBegin;
    toNext = toBegin;
    Char ch;

    size_t bytesToWrite;

    while(fromNext < fromEnd)
    {
        ch = *fromNext;
        if (ch >= SurHighStart && ch <= SurLowEnd)
        {
            retstat = error;
            break;
        }

        // Figure out how many bytes the result will require. Turn any
        // illegally large UTF32 things (> Plane 17) into replacement chars.
        if (ch < Char(0x80))
        {
            bytesToWrite = 1;
        }
        else if (ch < Char(0x800))
        {
            bytesToWrite = 2;
        }
        else if (ch < Char(0x10000))
        {
            bytesToWrite = 3;
        }
        else if (ch <= MaxLegalUtf32)
        {
            bytesToWrite = 4;
        }
        else
        {
            bytesToWrite = 3;
            ch = ReplacementChar;
        }

        uint8_t* current = (uint8_t*)(toNext + bytesToWrite);
        if( current >= (uint8_t*)(toEnd) )
        {
            retstat = partial;
            break;
        }

        Char::value_type chValue = ch.value();
        switch(bytesToWrite)
        { // note: everything falls through...
            case 4: *--current = static_cast<uint8_t>((chValue | byteMark) & byteMask); chValue >>= 6;
            case 3: *--current = static_cast<uint8_t>((chValue | byteMark) & byteMask); chValue >>= 6;
            case 2: *--current = static_cast<uint8_t>((chValue | byteMark) & byteMask); chValue >>= 6;
            case 1: *--current = static_cast<uint8_t> (chValue | firstByteMark[bytesToWrite]);
        }

        toNext += bytesToWrite;
        ++fromNext;
    }

    return retstat;
}


int Utf8Codec::do_length(MBState& /*s*/, const char* fromBegin, const char* fromEnd, size_t max) const
{
    const char* fromNext = fromBegin;
    size_t counter = 0;

    while(fromNext < fromEnd && counter <= max) {
        int extraBytesToRead = trailingBytesForUTF8[ (unsigned char)*fromNext ]; // NOTE: check again...

        if(fromNext + extraBytesToRead >= fromEnd) {
            break;
        }

        if(!isLegalUTF8( (const uint8_t*) fromNext, extraBytesToRead + 1 ) ) {
            break;
        }

        fromNext += extraBytesToRead + 1;
        counter += extraBytesToRead + 1;
    }

    return fromNext - fromBegin;
}


int Utf8Codec::do_max_length() const throw()
{
    return 4;
}


bool Utf8Codec::do_always_noconv() const throw()
{
    return false;
}

} // namespace cxxtools
