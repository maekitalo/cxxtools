/*
 * Copyright (C) 2013 by Tommi Maekitalo
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
#ifndef CXXTOOLS_ISO8859_1CODEC_H
#define CXXTOOLS_ISO8859_1CODEC_H

#include <cxxtools/api.h>
#include <cxxtools/char.h>
#include <cxxtools/textcodec.h>
#include <cxxtools/string.h>

namespace cxxtools
{

class CXXTOOLS_API Iso8859_1Codec : public TextCodec<Char, char>
{
    public:
        explicit Iso8859_1Codec(size_t ref = 0)
        : TextCodec<Char, char>(ref)
        {}

        virtual ~Iso8859_1Codec()
        {}

    protected:
        //! @brief Decodes ISO-8859-1 to UTF-32.
        virtual result do_in(MBState& s, const char* fromBegin,
                                        const char* fromEnd, const char*& fromNext,
                                        Char* toBegin, Char* toEnd, Char*& toNext) const;
        //! @brief Encodes UTF-32 to ISO-8859-1.
        virtual result do_out(MBState& s, const Char* fromBegin,
                                         const Char* fromEnd, const Char*& fromNext,
                                         char* toBegin, char* toEnd, char*& toNext) const;

        std::codecvt_base::result do_unshift(cxxtools::MBState&, char*, char*, char*&) const
        { return std::codecvt_base::noconv; }

        bool do_always_noconv() const throw();

        int do_length(MBState& s, const char* fromBegin,
                      const char* fromEnd, size_t max) const;

        int do_encoding() const throw()
        { return 0; }

        int do_max_length() const throw();

    public:
        /** @brief shortcut for converting base64 encoded data to std::string

            Example:
            @code
              std::string data = cxxtools::Iso8859_1Codec::decode(base64dataptr, base64datasize);
            @endcode
         */
        static String decode(const char* data, unsigned size)
        { return cxxtools::decode<Iso8859_1Codec>(data, size); }
        /** @brief shortcut for converting base64 encoded std::string to std::string
         */
        static String decode(const std::string& data)
        { return cxxtools::decode<Iso8859_1Codec>(data); }

        /** @brief shortcut for converting data to base64 encoded std::string
         */
        static std::string encode(const Char* data, unsigned size)
        { return cxxtools::encode<Iso8859_1Codec>(data, size); }

        /** @brief shortcut for converting std::string to base64 encoded std::string
         */
        static std::string encode(const String& data)
        { return cxxtools::encode<Iso8859_1Codec>(data); }
};


} //namespace cxxtools

#endif
