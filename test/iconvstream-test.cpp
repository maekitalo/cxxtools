/*
 * Copyright (C) 2012 Jiří Pinkava - Seznam.cz a. s.
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
#include "cxxtools/iconvstream.h"

class IconvstreamTest : public cxxtools::unit::TestSuite
{

    public:
        IconvstreamTest()
        : cxxtools::unit::TestSuite("iconvstream")
        {
            registerMethod("testIconvstream", *this, &IconvstreamTest::testIconvstream);
        }

        void testIconvstream()
        {
            const std::string src("iconv error for Ω test");
            const std::string res_cut("iconv error for ");
            const std::string res_skip("iconv error for  test");
            std::ostringstream out;
            cxxtools::iconvstreambuf ic;

            ic.open(out, "ascii", "UTF-8");
            ic.sputn(src.data(), src.size());
            ic.sync();
            CXXTOOLS_UNIT_ASSERT_EQUALS(out.str(), res_cut);

            out.str("");
            ic.open(out, "ascii", "UTF-8", cxxtools::iconvstreambuf::mode_skip);
            ic.sputn(src.data(), src.size());
            ic.sync();
            CXXTOOLS_UNIT_ASSERT_EQUALS(out.str(), res_skip);
            CXXTOOLS_UNIT_ASSERT_EQUALS(
                    ic.pubseekoff(0, std::ios_base::cur, std::ios_base::in),
                    res_skip.size());

            out.str("");
            ic.open(out, "ascii", "UTF-8", cxxtools::iconvstreambuf::mode_throw);
            ic.sputn(src.data(), src.size());
            CXXTOOLS_UNIT_ASSERT_THROW(ic.sync(), cxxtools::iconv_error);
            CXXTOOLS_UNIT_ASSERT_EQUALS(out.str(), res_cut);
        }
};

cxxtools::unit::RegisterTest<IconvstreamTest> register_IconvstreamTest;
