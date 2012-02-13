/*
 * Copyright (C) 2005 by Marc Boris Duerner
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
#ifndef cxxtools_Base64Codec_h
#define cxxtools_Base64Codec_h

#include <cxxtools/api.h>
#include <cxxtools/textcodec.h>

namespace cxxtools
{

class Base64Codec : public TextCodec<char, char>
{
    public:
        explicit Base64Codec(size_t ref = 0)
        : TextCodec<char, char>(ref)
        {}

        virtual ~Base64Codec()
        {}

    protected:
        result do_in(MBState& s,
                     const char* fromBegin,
                     const char* fromEnd,
                     const char*& fromNext,
                     char* toBegin,
                     char* toEnd,
                     char*& toNext) const;

        result do_out(MBState& s,
                      const char* fromBegin,
                      const char* fromEnd,
                      const char*& fromNext,
                      char* toBegin,
                      char* toEnd,
                      char*& toNext) const;

        result do_unshift(MBState& state,
                          char* toBegin,
                          char* toEnd,
                          char*& toNext) const;

        bool do_always_noconv() const throw()
        {
            return false;
        }

        int do_length(MBState& s, const char* fromBegin,
                      const char* fromEnd, size_t max) const
        {
            const int from = (fromEnd - fromBegin) / 4;
            const int to = max / 3;
            return to > from ? from * 4 : to * 4;
        }

        int do_encoding() const throw()
        {
            // stateful encoding
            return -1;
        }

        int do_max_length() const throw()
        {
            //worst case: XX== -> x
            return 4;
        }
};


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


inline Base64Codec::result Base64Codec::do_in(MBState& s,
                                       const char* fromBegin,
                                       const char* fromEnd,
                                       const char*& fromNext,
                                       char* toBegin,
                                       char* toEnd,
                                       char*& toNext) const
{
    fromNext = fromBegin;
    toNext = toBegin;

    while( (fromEnd - fromNext) >= 4 && (toEnd - toNext) >= 3 )
    {
        uint8_t first  = fromBase64( *(fromNext++) );
        uint8_t second = fromBase64( *(fromNext++) );
        uint8_t third  = fromBase64( *(fromNext++) );
        uint8_t fourth = fromBase64( *(fromNext++) );

        *(toNext++) = (first << 2) + (second >> 4);

        if(third != 64)
            *(toNext++) = (second << 4) + (third >> 2);

        if(fourth != 64)
            *(toNext++) = (third << 6) + (fourth);
    }

    if( fromEnd == fromNext )
        return std::codecvt_base::ok;

    return std::codecvt_base::partial;
}


inline Base64Codec::result Base64Codec::do_out(cxxtools::MBState& state,
                                        const char* fromBegin,
                                        const char* fromEnd,
                                        const char*& fromNext,
                                        char* toBegin,
                                        char* toEnd,
                                        char*& toNext) const
{
    fromNext = fromBegin;
    toNext = toBegin;

    const char* first = 0;
    const char* second = 0;
    const char* third = 0;

    if(fromEnd - fromNext < 1)
        return std::codecvt_base::partial;

    if(toEnd - toNext < 4)
        return std::codecvt_base::partial;

    switch( state.n )
    {
        case 2:
            first  = &state.value.mbytes[0];
            second = &state.value.mbytes[1];
            third  = fromNext++;
            break;

        case 1:
            if(fromEnd - fromNext < 2)
            {
                state.value.mbytes[1] = *fromNext++;
                state.n = 2;
                return std::codecvt_base::partial;
            }

            first  = &state.value.mbytes[0];
            second = fromNext++;
            third  = fromNext++;
            break;

        default:
            first  = fromNext++;
            second = fromNext++;
            third  = fromNext++;
            break;
    }

    while (true)
    {
        *toNext++   = toBase64( (*first >> 2) & 0x3f );
        *(toNext++) = toBase64( ((*first << 4) + ((*second) >> 4)) & 0x3f );
        *(toNext++) = toBase64( ((*second << 2) + ((*third) >> 6)) & 0x3f );
        *(toNext++) = toBase64( *third & 0x3f );

        if(toEnd - toNext < 4)
            return std::codecvt_base::partial;

        if( fromEnd - fromNext < 3 )
            break;

        first =  fromNext++;
        second = fromNext++;
        third =  fromNext++;
    }

    switch( fromEnd - fromNext )
    {
        case 2:
            state.value.mbytes[0] = *fromNext++;
            state.value.mbytes[1] = *fromNext++;
            state.n = 2;
            break;

        case 1:
            state.value.mbytes[0] = *fromNext++;
            state.n = 1;
            break;

        default:
            state = MBState();
            break;
    }

    return std::codecvt_base::ok;
}


inline Base64Codec::result Base64Codec::do_unshift(MBState& state,
                                            char* toBegin,
                                            char* toEnd,
                                            char*& toNext) const
{
    toNext = toBegin;

    if(toEnd - toBegin < 4)
    {
        return std::codecvt_base::partial;
    }

    switch(state.n)
    {
        case 2:
            *toNext++   = toBase64( (state.value.mbytes[0] >> 2) & 0x3f );
            *(toNext++) = toBase64( ((state.value.mbytes[0] << 4) + ((state.value.mbytes[1]) >> 4)) & 0x3f );
            *(toNext++) = toBase64( (state.value.mbytes[1] << 2) &  0x3f );
            *(toNext++) = '=';
            break;

        case 1:
            *toNext++   = toBase64( (state.value.mbytes[0] >> 2) & 0x3f );
            *(toNext++) = toBase64( (state.value.mbytes[0] << 4) & 0x3f );
            *(toNext++) = '=';
            *(toNext++) = '=';
            break;

        case 0:
            return std::codecvt_base::noconv;
    }

    state = MBState();
    return std::codecvt_base::ok;
}

} //namespace cxxtools

#endif
