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

#include <cxxtools/textcodec.h>
#include <cxxtools/string.h>

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

    public:
        /** @brief shortcut for converting base64 encoded data to std::string

            Example:
            @code
              std::string data = cxxtools::Base64Codec::decode(base64dataptr, base64datasize);
            @endcode
         */
        static std::string decode(const char* data, unsigned size)
        { return cxxtools::decode<Base64Codec>(data, size); }
        /** @brief shortcut for converting base64 encoded std::string to std::string
         */
        static std::string decode(const std::string& data)
        { return cxxtools::decode<Base64Codec>(data); }

        /** @brief shortcut for converting data to base64 encoded std::string
         */
        static std::string encode(const char* data, unsigned size)
        { return cxxtools::encode<Base64Codec>(data, size); }

        /** @brief shortcut for converting std::string to base64 encoded std::string
         */
        static std::string encode(const std::string& data)
        { return cxxtools::encode<Base64Codec>(data); }
};


} //namespace cxxtools

#endif
