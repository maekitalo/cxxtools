/*
 * Copyright (C) 2018 Tommi Maekitalo
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

#include <cxxtools/ptrstream.h>
#include "cxxtools/unit/testsuite.h"
#include "cxxtools/unit/registertest.h"

class PtrStreamTest : public cxxtools::unit::TestSuite
{
    public:
        PtrStreamTest()
        : cxxtools::unit::TestSuite("ptrstream")
        {
            registerMethod("testPtrIStream", *this, &PtrStreamTest::testPtrIStream);
            registerMethod("testPtrOStream", *this, &PtrStreamTest::testPtrOStream);
        }

        void testPtrIStream()
        {
            char data[] = "hi";

            cxxtools::PtrStream in(data, data + 2);

            char ch = '\0';

            in.get(ch);
            CXXTOOLS_UNIT_ASSERT(!in.eof());
            CXXTOOLS_UNIT_ASSERT_EQUALS(ch, 'h');

            in.get(ch);
            CXXTOOLS_UNIT_ASSERT(!in.eof());
            CXXTOOLS_UNIT_ASSERT_EQUALS(ch, 'i');

            in.get(ch);
            CXXTOOLS_UNIT_ASSERT(in.eof());
        }

        void testPtrOStream()
        {
            char data[2] = "\0";

            cxxtools::PtrStream out(data, data + 2);

            CXXTOOLS_UNIT_ASSERT(out.begin() == &data[0]);
            CXXTOOLS_UNIT_ASSERT(out.end() == &data[0]);

            out << "hi";
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[0], 'h');
            CXXTOOLS_UNIT_ASSERT_EQUALS(data[1], 'i');
            CXXTOOLS_UNIT_ASSERT(!out.fail());

            out << 'h';
            CXXTOOLS_UNIT_ASSERT(out.fail());

            CXXTOOLS_UNIT_ASSERT(out.begin() == &data[0]);
            CXXTOOLS_UNIT_ASSERT(out.end() == &data[2]);
        }
};

cxxtools::unit::RegisterTest<PtrStreamTest> register_PtrStreamTest;
