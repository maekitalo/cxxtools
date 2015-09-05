/*
 * Copyright (C) 2012 Tommi Maekitalo
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

#include <cxxtools/base64codec.h>

namespace cxxtools
{

namespace
{

inline char toBase64(uint8_t n)
{
    static const char b64enc[]
        = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    return b64enc[n];
}


inline uint8_t fromBase64(char b64)
{
    static const uint8_t b64dec[]
        = { 255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
            255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
            255,255,255,255,255,255,255,255,255,255,255,62,255,255,255,63,
            52,53,54,55,56,57,58,59,60,61,255,255,255,64,255,255,
            255, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,
            15,16,17,18,19,20,21,22,23,24,25,255,255,255,255,255,
            255,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,
            41,42,43,44,45,46,47,48,49,50,51,255,255,255,255,255,
            255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
            255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
            255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
            255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
            255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
            255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
            255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
            255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255 };

    return b64dec[(int)b64];
}

}


Base64Codec::result Base64Codec::do_in(MBState& /*s*/,
                                       const char* fromBegin,
                                       const char* fromEnd,
                                       const char*& fromNext,
                                       char* toBegin,
                                       char* toEnd,
                                       char*& toNext) const
{
    fromNext = fromBegin;
    toNext = toBegin;

    while ( (fromEnd - fromNext) >= 4 && (toEnd - toNext) >= 3 )
    {
        uint8_t first  = fromBase64( *(fromNext++) );
        uint8_t second = fromBase64( *(fromNext++) );
        uint8_t third  = fromBase64( *(fromNext++) );
        uint8_t fourth = fromBase64( *(fromNext++) );

        *(toNext++) = (first << 2) + (second >> 4);

        if (third != 64)
            *(toNext++) = (second << 4) + (third >> 2);

        if (fourth != 64)
            *(toNext++) = (third << 6) + (fourth);
    }

    if ( fromEnd == fromNext )
        return std::codecvt_base::ok;

    return std::codecvt_base::partial;
}


Base64Codec::result Base64Codec::do_out(cxxtools::MBState& state,
                                        const char* fromBegin,
                                        const char* fromEnd,
                                        const char*& fromNext,
                                        char* toBegin,
                                        char* toEnd,
                                        char*& toNext) const
{
    fromNext = fromBegin;
    toNext = toBegin;

    while (fromEnd - fromNext > 0)
    {
        if (state.n == 3)
        {
            if (toEnd - toNext < 4)
                return std::codecvt_base::partial;

            *toNext++   = toBase64( (static_cast<unsigned char>(state.value.mbytes[0]) >> 2) & 0x3f );
            *(toNext++) = toBase64( ((static_cast<unsigned char>(state.value.mbytes[0]) << 4)
                                        + (static_cast<unsigned char>(state.value.mbytes[1]) >> 4)) & 0x3f );
            *(toNext++) = toBase64( (static_cast<unsigned char>(state.value.mbytes[1] << 2)
                                        + (static_cast<unsigned char>(state.value.mbytes[2]) >> 6)) & 0x3f );
            *(toNext++) = toBase64( static_cast<unsigned char>(state.value.mbytes[2]) & 0x3f );
            state.n = 0;
        }

        state.value.mbytes[state.n++] = *fromNext++;
    }

    return std::codecvt_base::ok;
}


Base64Codec::result Base64Codec::do_unshift(MBState& state,
                                            char* toBegin,
                                            char* toEnd,
                                            char*& toNext) const
{
    toNext = toBegin;

    if (toEnd - toBegin < 4)
        return std::codecvt_base::partial;

    switch(state.n)
    {
        case 3:
            *toNext++   = toBase64( (static_cast<unsigned char>(state.value.mbytes[0]) >> 2) & 0x3f );
            *(toNext++) = toBase64( ((static_cast<unsigned char>(state.value.mbytes[0]) << 4)
                                        + (static_cast<unsigned char>(state.value.mbytes[1]) >> 4)) & 0x3f );
            *(toNext++) = toBase64( (static_cast<unsigned char>(state.value.mbytes[1] << 2)
                                        + (static_cast<unsigned char>(state.value.mbytes[2]) >> 6)) & 0x3f );
            *(toNext++) = toBase64( static_cast<unsigned char>(state.value.mbytes[2]) & 0x3f );
            break;

        case 2:
            *toNext++   = toBase64( (static_cast<unsigned char>(state.value.mbytes[0]) >> 2) & 0x3f );
            *(toNext++) = toBase64( ((static_cast<unsigned char>(state.value.mbytes[0]) << 4)
                                        + (static_cast<unsigned char>(state.value.mbytes[1]) >> 4)) & 0x3f );
            *(toNext++) = toBase64( (static_cast<unsigned char>(state.value.mbytes[1]) << 2) &  0x3f );
            *(toNext++) = '=';
            break;

        case 1:
            *toNext++   = toBase64( (static_cast<unsigned char>(state.value.mbytes[0]) >> 2) & 0x3f );
            *(toNext++) = toBase64( (static_cast<unsigned char>(state.value.mbytes[0]) << 4) & 0x3f );
            *(toNext++) = '=';
            *(toNext++) = '=';
            break;

        case 0:
            return std::codecvt_base::noconv;
    }

    state = MBState();
    return std::codecvt_base::ok;
}

}
