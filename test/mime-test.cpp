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

#include "cxxtools/unit/testsuite.h"
#include "cxxtools/unit/registertest.h"
#include "cxxtools/mime.h"
#include "cxxtools/serializationinfo.h"
#include <sstream>

class MimeTest : public cxxtools::unit::TestSuite
{
    public:
        MimeTest()
        : cxxtools::unit::TestSuite("mime")
        {
            registerMethod("parseMessage", *this, &MimeTest::parseMessage);
            registerMethod("buildMessage", *this, &MimeTest::buildMessage);
            registerMethod("serializeMessage", *this, &MimeTest::serializeMessage);
            registerMethod("multipartMessage", *this, &MimeTest::multipartMessage);
            registerMethod("outputMessage", *this, &MimeTest::outputMessage);
            registerMethod("serializeMultipartMessage", *this, &MimeTest::serializeMultipartMessage);
            registerMethod("serializeMultipartToEntity", *this, &MimeTest::serializeMultipartToEntity);
        }

        void parseMessage()
        {
            std::string msg(
                "foo: bar\r\n"
                "bar: \r\n"
                " blub\r\n\r\nbody");

            cxxtools::MimeEntity mime(msg);
            CXXTOOLS_UNIT_ASSERT_EQUALS(mime.getHeader("FOO"), "bar");
            CXXTOOLS_UNIT_ASSERT_EQUALS(mime.getHeader("baR"), " blub");
            CXXTOOLS_UNIT_ASSERT_EQUALS(mime.getBody(), "body");
        }

        void buildMessage()
        {
            cxxtools::MimeEntity mime;
            mime.addHeader("hdr1", "bar1");
            mime.addHeader("hdr2", "bar2");
            mime.getBody() = "some body text";

            CXXTOOLS_UNIT_ASSERT_EQUALS(mime.getHeader("hdr1"), "bar1");
            CXXTOOLS_UNIT_ASSERT_EQUALS(mime.getHeader("hdr2"), "bar2");
            CXXTOOLS_UNIT_ASSERT_EQUALS(mime.getBody(), "some body text");
        }

        void outputMessage()
        {
            cxxtools::MimeEntity mime;
            mime.addHeader("hdr1", "bar1\nblub");
            mime.addHeader("hdr2", "bar2");
            mime.getBody() = "some body text";

            std::ostringstream s;
            s << mime;

            CXXTOOLS_UNIT_ASSERT_EQUALS(s.str(), "MIME-Version: 1.0\r\nhdr1: bar1\n blub\r\nhdr2: bar2\r\n\r\nsome body text");
        }

        void serializeMessage()
        {
            cxxtools::SerializationInfo si;

            {
                cxxtools::MimeEntity mime;
                mime.addHeader("hdr1", "bar1");
                mime.addHeader("hdr2", "bar2");
                mime.getBody() = "some body text";

                si <<= mime;
            }

            {
                cxxtools::MimeEntity mime2;
                si >>= mime2;

                CXXTOOLS_UNIT_ASSERT_EQUALS(mime2.getHeader("hdr1"), "bar1");
                CXXTOOLS_UNIT_ASSERT_EQUALS(mime2.getHeader("hdr2"), "bar2");
                CXXTOOLS_UNIT_ASSERT_EQUALS(mime2.getBody(), "some body text");
            }
        }

        void multipartMessage()
        {
            cxxtools::MimeMultipart mp;
            mp.addObject();
            mp.addObject();
            CXXTOOLS_UNIT_ASSERT_EQUALS(mp.size(), 2);
        }

        void serializeMultipartMessage()
        {
            cxxtools::SerializationInfo si;

            {
                cxxtools::MimeMultipart mp;
                mp.addObject();
                mp.addObject();

                si <<= mp;
            }

            {
                cxxtools::MimeMultipart mp;
                si >>= mp;

                CXXTOOLS_UNIT_ASSERT_EQUALS(mp.size(), 2);
            }
        }

        void serializeMultipartToEntity()
        {
            cxxtools::MimeMultipart mp;
            mp.addObject();
            mp.addObject();

            cxxtools::SerializationInfo si;
            si <<= mp;

            cxxtools::MimeEntity me;
            si >>= me;

            CXXTOOLS_UNIT_ASSERT(me.isMultipart());

            cxxtools::MimeMultipart mp2(me);
            CXXTOOLS_UNIT_ASSERT_EQUALS(mp2.size(), 2);
        }

};

cxxtools::unit::RegisterTest<MimeTest> register_MimeTest;
