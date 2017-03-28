/*
 * Copyright (C) 2016 Tommi Maekitalo
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

#include <cxxtools/quotedprintablecodec.h>
#include <cstring>

namespace cxxtools
{

namespace
{
// returns the next byte
char readByte(MBState& s, const char*& fromNext)
{
    // note, that s.n = 0 means uninitialized, hence the s.n increased by one
    if (s.n > 1)
    {
        char ret = s.value.mbytes[0];
        std::memmove(s.value.mbytes, s.value.mbytes + 1, s.n - 2);
        --s.n;
        return ret;
    }
    else
    {
        return *fromNext++;
    }
}

char peekByte(const MBState& s, const char* fromNext)
{
    // note, that s.n = 0 means uninitialized, hence the s.n increased by one
    if (s.n > 1)
    {
        return s.value.mbytes[0];
    }
    else
    {
        return *fromNext;
    }
}

inline unsigned available(const MBState& s, const char* fromNext, const char* fromEnd)
{
    return s.n - 1 + (fromEnd - fromNext);
}

inline bool isqprint(char ch)
{
    return (ch >= 33 && ch <= 60)
        || (ch >= 62 && ch <= 126);
}

}


QuotedPrintableCodec::result QuotedPrintableCodec::do_in(cxxtools::MBState& s,
                                        const char* fromBegin,
                                        const char* fromEnd,
                                        const char*& fromNext,
                                        char* toBegin,
                                        char* toEnd,
                                        char*& toNext) const
{
    fromNext = fromBegin;
    toNext = toBegin;

    enum {
        state_0,
        state_eq,
        state_cr,
        state_b1,
        state_b2,
    };

    // we misuse the MBState:
    //  s.n = 0 => uninitialized
    //  s.value.mbytes[0] = current incomplete char
    //  s.value.mbytes[1] = parsing state
    unsigned char& currentChar = reinterpret_cast<unsigned char&>(s.value.mbytes[0]);
    unsigned char& state = reinterpret_cast<unsigned char&>(s.value.mbytes[1]);
    if (s.n == 0)
    {
        s.n = 1;
        currentChar = 0;
        state = state_0;
    }

    while (fromNext < fromEnd && toEnd > toNext)
    {
        char ch = *fromNext++;
        switch (state)
        {
            case state_0:
                if (ch == '=')
                    state = state_eq;
                else
                    *toNext++ = ch;
                break;

            case state_eq:
                if (ch >= '0' && ch <= '9')
                {
                    currentChar = (ch - '0') << 4;
                    state = state_b1;
                }
                else if (ch >= 'A' && ch <= 'F')
                {
                    currentChar = (ch - 'A' + 10) << 4;
                    state = state_b1;
                }
                else if (ch == '\r')
                    state = state_cr;
                else if (ch == '\n')
                    state = state_0;
                else
                    return std::codecvt_base::error;
                break;

            case state_cr:
                if (ch == '\n')
                    state = state_0;
                else if (ch == '=')
                    state = state_eq;
                else
                    *toNext++ = ch;
                state = state_0;
                break;

            case state_b1:
                if (ch >= '0' && ch <= '9')
                    currentChar |= (ch - '0') & 0xf;
                else if (ch >= 'A' && ch <= 'F')
                    currentChar |= (ch - 'A' + 10) & 0xf;
                else
                    return std::codecvt_base::error;

                *toNext++ = currentChar;
                state = state_0;
                break;
        }
    }

    return fromNext < fromEnd
                ? std::codecvt_base::partial
                : std::codecvt_base::ok;
}


// convert quoted printable into bytes
QuotedPrintableCodec::result QuotedPrintableCodec::do_out(MBState& s,
                                       const char* fromBegin,
                                       const char* fromEnd,
                                       const char*& fromNext,
                                       char* toBegin,
                                       char* toEnd,
                                       char*& toNext) const
{
    static const char hex[17] = "0123456789ABCDEF";

    fromNext = fromBegin;
    toNext = toBegin;

    // we misuse the MBState:
    //  s.n = 0 => uninitialized
    //  when s.n > 0 then the last char (s.value.mbytes[15]) indicates the current column

    static const unsigned char maxcol = 76;
    unsigned char& col = reinterpret_cast<unsigned char&>(s.value.mbytes[15]);
    if (s.n == 0)
    {
        s.n = 1;
        col = 0;
    }

    while (available(s, fromNext, fromEnd) > 0 && toEnd > toNext)
    {
        if (col > maxcol && toEnd - toNext > 3)
        {
            // There is one problem here: when we do never get enough space
            // to write the soft line break, we never get a chance to write
            // it. Then we will break the rule, that a line must not have more
            // than 76 characters.
            *toNext++ = '=';
            *toNext++ = '\r';
            *toNext++ = '\n';
            col = 0;
        }

        char ch = readByte(s, fromNext);
        if (isqprint(ch))
        {
            *toNext++ = ch;
            ++col;
        }
        else if (ch == '\r' || ch == '\n')
        {
            *toNext++ = ch;
            col = 0;
        }
        else if (ch == '\t' || ch == ' ')
        {
            // tab or space is not allowed at the end of the line
            if (available(s, fromNext, fromEnd))
            {
                char next = peekByte(s, fromNext);
                if (next != '\n' && next != '\r')
                {
                    *toNext++ = ch;
                    ++col;
                }
                else if (toEnd >= toNext + 3 && col < maxcol - 3)
                {
                    // translate to hex sequence
                    *toNext++ = '=';
                    *toNext++ = hex[static_cast<unsigned char>(ch) >> 4];
                    *toNext++ = hex[static_cast<unsigned char>(ch) & 0xf];
                    col += 3;
                }
                else
                {
                    // no space in output sequence => store char
                    s.value.mbytes[s.n-1] = ch;
                    ++s.n;
                    return std::codecvt_base::partial;
                }
            }
            else
            {
                // don't know, whether the next char is printable - store for later use
                s.value.mbytes[s.n-1] = ch;
                ++s.n;
                return std::codecvt_base::partial;
            }
        }
        else if (toNext + 3 < toEnd)
        {
            if (col + 3 > maxcol)
            {
                *toNext++ = '=';
                *toNext++ = '\r';
                *toNext++ = '\n';
                col = 0;

                if (toEnd < toNext + 3)
                {
                    // we filled the remaining space withour newline
                    // put the char back to buffer and wait for a new output buffer
                    s.value.mbytes[s.n-1] = ch;
                    ++s.n;
                    return std::codecvt_base::partial;
                }
            }

            *toNext++ = '=';
            *toNext++ = hex[static_cast<unsigned char>(ch) >> 4];
            *toNext++ = hex[static_cast<unsigned char>(ch) & 0xf];
            col += 3;
        }
        else
        {
            s.value.mbytes[s.n-1] = ch;
            ++s.n;
            return std::codecvt_base::partial;
        }
    }

    return available(s, fromNext, fromEnd)
                ? std::codecvt_base::partial
                : std::codecvt_base::ok;
}


QuotedPrintableCodec::result QuotedPrintableCodec::do_unshift(MBState& s,
                                            char* toBegin,
                                            char* toEnd,
                                            char*& toNext) const
{
    const char* fromBegin = 0;
    const char* fromEnd = 0;
    const char* fromNext = 0;
    return do_out(s, fromBegin, fromEnd, fromNext, toBegin, toEnd, toNext);
}

}
