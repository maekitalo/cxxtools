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
#include <cxxtools/conversionerror.h>

#define halfShift uint32_t(10)
#define halfBase cxxtools::Char(0x0010000)
#define halfMask cxxtools::Char(0x3FF)
#define byteMask cxxtools::Char(0xBF)
#define byteMark cxxtools::Char(0x80)

namespace cxxtools {

const cxxtools::Char ReplacementChar = 0x0000FFFD;
const cxxtools::Char MaxBmp = 0x0000FFFF;
const cxxtools::Char MaxUtf16 = 0x0010FFFF;
const cxxtools::Char MaxUtf32 = 0x7FFFFFFF;
const cxxtools::Char MaxLegalUtf32 = 0x0010FFFF;
const cxxtools::Char SurHighStart = 0xD800;
const cxxtools::Char SurHighEnd = 0xDBFF;
const cxxtools::Char SurLowStart = 0xDC00;
const cxxtools::Char SurLowEnd = 0xDFFF;
const cxxtools::Char ByteOrderMark = 0xFEFF;
const cxxtools::Char ByteOrderSwapped = 0xFFFE;

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
const Char offsetsFromUTF8[6] = {
    0x00000000UL,
    0x00003080UL,
    0x000E2080UL,
    0x03C82080UL,
    0xFA082080UL,
    0x82082080UL
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
: cxxtools::TextCodec<Char, char>(ref)
{}


Utf8Codec::result Utf8Codec::do_in(MBState& s, const char* fromBegin, const char* fromEnd, const char*& fromNext,
                                   cxxtools::Char* toBegin, cxxtools::Char* toEnd, cxxtools::Char*& toNext) const
{
    Utf8Codec::result retstat = ok;
    fromNext = fromBegin;
    toNext = toBegin;

    while(fromNext < fromEnd) {
        uint8_t* fnext = (uint8_t *)(fromNext);

        if(toNext >= toEnd) {
            retstat = partial;
            break;
        }

        const size_t extraBytesToRead = trailingBytesForUTF8[*fnext];
        if(fromNext + extraBytesToRead >= fromEnd) {
            retstat = partial;
            break;
        }

        if( !isLegalUTF8( (const uint8_t*)fnext, extraBytesToRead + 1 ) ) {
            retstat = error;
            break;
        }

        *toNext = 0;
        switch (extraBytesToRead) {
            case 5: *toNext += *fnext++; *toNext <<= 6; // We should never get this for legal UTF-8
            case 4: *toNext += *fnext++; *toNext <<= 6; // We should never get this for legal UTF-8
            case 3: *toNext += *fnext++; *toNext <<= 6;
            case 2: *toNext += *fnext++; *toNext <<= 6;
            case 1: *toNext += *fnext++; *toNext <<= 6;
            case 0: *toNext += *fnext++;
        }
        *toNext -= offsetsFromUTF8[extraBytesToRead];

        // UTF-16 surrogate values are illegal in UTF-32, and anything
        // over Plane 17 (> 0x10FFFF) is illegal.
        if(*toNext > MaxLegalUtf32) {
            *toNext = ReplacementChar;
        }
        else if(*toNext >= SurHighStart && *toNext <= SurLowEnd) {
            *toNext = ReplacementChar;
        }

        ++toNext;
        fromNext += (extraBytesToRead + 1);
    }

    return retstat;
}


Utf8Codec::result Utf8Codec::do_out(MBState& s, const cxxtools::Char* fromBegin, const cxxtools::Char* fromEnd, const cxxtools::Char*& fromNext,
                                                  char* toBegin, char* toEnd, char*& toNext) const
{
    result retstat = ok;
    fromNext  = fromBegin;
    toNext = toBegin;
    cxxtools::Char ch;

    size_t bytesToWrite;

    while(fromNext < fromEnd) {
        ch = *fromNext;
        if (ch >= SurHighStart && ch <= SurLowEnd) {
            retstat = error;
            break;
        }

        // Figure out how many bytes the result will require. Turn any
        // illegally large UTF32 things (> Plane 17) into replacement chars.
        if (ch < cxxtools::Char(0x80)) {
            bytesToWrite = 1;
        }
        else if (ch < cxxtools::Char(0x800)) {
            bytesToWrite = 2;
        }
        else if (ch < cxxtools::Char(0x10000)) {
            bytesToWrite = 3;
        }
        else if (ch <= MaxLegalUtf32) {
            bytesToWrite = 4;
        }
        else {
            bytesToWrite = 3;
            ch = ReplacementChar;
        }

        uint8_t* current = (uint8_t*)(toNext + bytesToWrite);
        if( current >= (uint8_t*)(toEnd) ) {
            retstat = partial;
            break;
        }

        switch(bytesToWrite) { // note: everything falls through...
            case 4: *--current = (uint8_t)((ch | byteMark) & byteMask).value(); ch >>= 6;
            case 3: *--current = (uint8_t)((ch | byteMark) & byteMask).value(); ch >>= 6;
            case 2: *--current = (uint8_t)((ch | byteMark) & byteMask).value(); ch >>= 6;
            case 1: *--current = (uint8_t) (ch.value() | firstByteMark[bytesToWrite]);
        }

        toNext += bytesToWrite;
        ++fromNext;
    }

    return retstat;
}


int Utf8Codec::do_length(MBState& s, const char* fromBegin, const char* fromEnd, size_t max) const
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

String Utf8Codec::decode(const char* data, unsigned size)
{
    Utf8Codec codec;

    Char to[16];
    MBState state;
    String ret;
    const char* from = data;

    result r;
    do
    {
        Char* to_next = to;

        const char* from_next = from;
        r = codec.in(state, from, from + size, from_next, to, &to[sizeof(to) / sizeof(Char)], to_next);

        if (r == error)
            throw ConversionError("character conversion failed");

        ret.append(to, to_next);

        size -= (from_next - from);
        from = from_next;

    } while (r == partial);

    return ret;
}

std::string Utf8Codec::encode(const Char* data, unsigned size)
{
    Utf8Codec codec;
    char to[16];
    MBState state;
    
    result r;
    const Char* from = data;
    std::string ret;

    do{
        const Char* from_next;

        char* to_next = to;
        r = codec.out(state, from, from + size, from_next, to, to + sizeof(to), to_next);

        if (r == error)
            throw ConversionError("character conversion failed");

        ret.append(to, to_next);

        size -= (from_next - from);
        from = from_next;

    } while (r == partial);

    return ret;
}

} // namespace cxxtools
