/*
 * Copyright (C) 2010 Tommi Maekitalo
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

#include <iostream>
#include "cxxtools/base64stream.h"
#include "cxxtools/unit/testsuite.h"
#include "cxxtools/unit/registertest.h"
#include "cxxtools/log.h"

log_define("cxxtools.test.base64")

class Base64Test : public cxxtools::unit::TestSuite
{
        static std::string encodeDecode(const std::string& data)
        {
            std::string b = cxxtools::encode<cxxtools::Base64Codec>(data);
            log_debug("data.size=" << data.size() << " data=\"" << data << "\" base64.size=" << b.size() << " base64=" << b);
            return cxxtools::decode<cxxtools::Base64Codec>(b);
        }

    public:
        Base64Test()
        : cxxtools::unit::TestSuite("base64")
        {
            registerMethod("encodeTest0", *this, &Base64Test::encodeTest0);
            registerMethod("encodeTest1", *this, &Base64Test::encodeTest1);
            registerMethod("encodeTest2", *this, &Base64Test::encodeTest2);
            registerMethod("encodeStreamTest0", *this, &Base64Test::encodeStreamTest0);
            registerMethod("encodeStreamTest1", *this, &Base64Test::encodeStreamTest1);
            registerMethod("encodeStreamTest2", *this, &Base64Test::encodeStreamTest2);
            registerMethod("encodeDecodeTest", *this, &Base64Test::encodeDecodeTest);
            registerMethod("binaryTest", *this, &Base64Test::binaryTest);
            registerMethod("artemTest", *this, &Base64Test::artemTest);
        }

        void encodeTest0()
        {
            std::string b64 = cxxtools::encode<cxxtools::Base64Codec>("123456789");
            CXXTOOLS_UNIT_ASSERT_EQUALS(b64, "MTIzNDU2Nzg5");
        }

        void encodeTest1()
        {
            std::string b64 = cxxtools::encode<cxxtools::Base64Codec>("1234567890");
            CXXTOOLS_UNIT_ASSERT_EQUALS(b64, "MTIzNDU2Nzg5MA==");
        }

        void encodeTest2()
        {
            std::string b64 = cxxtools::encode<cxxtools::Base64Codec>("12345678901");
            CXXTOOLS_UNIT_ASSERT_EQUALS(b64, "MTIzNDU2Nzg5MDE=");
        }

        void encodeStreamTest0()
        {
            std::ostringstream s;
            
            cxxtools::Base64ostream encoder(s);
            encoder << "123456789";
            encoder.terminate();

            CXXTOOLS_UNIT_ASSERT_EQUALS(s.str(), "MTIzNDU2Nzg5");
        }

        void encodeStreamTest1()
        {
            std::ostringstream s;
            
            cxxtools::Base64ostream encoder(s);
            encoder << "1234567890";
            encoder.terminate();

            CXXTOOLS_UNIT_ASSERT_EQUALS(s.str(), "MTIzNDU2Nzg5MA==");
        }

        void encodeStreamTest2()
        {
            std::ostringstream s;

            cxxtools::Base64ostream encoder(s);
            encoder << "12345678901";
            encoder.terminate();

            CXXTOOLS_UNIT_ASSERT_EQUALS(s.str(), "MTIzNDU2Nzg5MDE=");
        }

        void encodeDecodeTest()
        {
            std::string data;
            for (unsigned n = 0; n < 100; ++n)
            {
                data += static_cast<char>('0' + n%10);
                for (char c = 'A'; c <= 'Z'; ++c)
                    data += c;
                data += '\n';
            }

            std::string data2 = encodeDecode(data);

            CXXTOOLS_UNIT_ASSERT_EQUALS(data, data2);
        }

        void binaryTest()
        {
            std::string data2;
            std::string data;
            
            data.assign("\xff\xd8\xff\xe0\x00\x10\x4a\x46\x00\x01\x01\x00\x48", 16);
            data2 = encodeDecode(data);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data, data2);

            data.assign("\xef\xfe\xff\xca", 4);
            data2 = encodeDecode(data);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data, data2);

            data.assign("\xef\xfe\xff\xca\xea", 5);
            data2 = encodeDecode(data);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data, data2);

            data.assign("\xef\xfe\xff\xca\xea\xcc", 6);
            data2 = encodeDecode(data);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data, data2);

            data.assign("\xff\xd8\xff\xe0\x0\x10\x4a\x46\x49\x46"
              "\x0\x1\x1\x1\x0\x48\x0\x48\x0\x0"
              "\xff\xfe\x0\xd\x4c\x61\x76\x63\x35\x33"
              "\x2e\x34\x32\x2e\x34\xff\xdb\x0\x43\x0"
              "\x5\x3\x4\x4\x4\x3\x5\x4\x4\x4"
              "\x5\x5\x5\x6\x7\xc\x8\x7\x7\x7"
              "\x7\xf\xb\xb\x9\xc\x11\xf\x12\x12"
              "\x11\xf\x11\x11\x13\x16\x1c\x17\x13\x14"
              "\x1a\x15\x11\x11\x18\x21\x18\x1a\x1d\x1d"
              "\x1f\x1f\x1f\x13\x17\x22\x24\x22\x1e\x24"
              "\x1c\x1e\x1f\x1e\xff\xdb\x0\x43\x1\x5"
              "\x5\x5\x7\x6\x7\xe\x8\x8\xe\x1e"
              "\x14\x11\x14\x1e\x1e\x1e\x1e\x1e\x1e\x1e"
              "\x1e\x1e\x1e\x1e\x1e\x1e\x1e\x1e\x1e\x1e"
              "\x1e\x1e\x1e\x1e\x1e\x1e\x1e\x1e\x1e\x1e"
              "\x1e\x1e\x1e\x1e\x1e\x1e\x1e\x1e\x1e\x1e"
              "\x1e\x1e\x1e\x1e\x1e\x1e\x1e\x1e\x1e\x1e"
              "\x1e\x1e\x1e\xff\xc0\x0\x11\x8\x0\x12"
              "\x0\x17\x3\x1\x22\x0\x2\x11\x1\x3"
              "\x11\x1\xff\xc4\x0\x18\x0\x1\x1\x1"
              "\x1\x1\x0\x0\x0\x0\x0\x0\x0\x0"
              "\x0\x0\x0\x0\x2\x4\x3\x7\xff\xc4"
              "\x0\x21\x10\x0\x1\x4\x1\x3\x5\x0"
              "\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0"
              "\x1\x2\x3\x12\x21\x4\x11\x22\x13\x31"
              "\x32\x42\x43\xff\xc4\x0\x17\x1\x1\x1"
              "\x1\x1\x0\x0\x0\x0\x0\x0\x0\x0"
              "\x0\x0\x0\x0\x0\x2\x1\x4\xff\xc4"
              "\x0\x15\x11\x1\x1\x0\x0\x0\x0\x0"
              "\x0\x0\x0\x0\x0\x0\x0\x0\x0\x0"
              "\x11\xff\xda\x0\xc\x3\x1\x0\x2\x11"
              "\x3\x11\x0\x3f\x0\xf4\x5e\xad\x79\x6d"
              "\x9f\x53\x2b\xe5\xce\x6a\x44\xee\xcb\x50"
              "\x9a\xdd\x76\x79\xd3\x5\x49\xa9\x8d\xa9"
              "\xe7\xc8\x13\x2c\x51\xba\xb8\x6\xd\x33"
              "\x22\x1c\x7e\xcc\x0\xa0\x7f\x60\x1\x23"
              "\xff\xd9", 362);
 
            data2 = encodeDecode(data);
            CXXTOOLS_UNIT_ASSERT_EQUALS(data, data2);
        }

        void artemTest()
        {
            for (unsigned n = 0; n < 256; ++n)
            {
                std::string data(n, 'a');
                std::string data2 = encodeDecode(data);
                CXXTOOLS_UNIT_ASSERT_EQUALS(data, data2);
            }
        }
};

cxxtools::unit::RegisterTest<Base64Test> register_Base64Test;
