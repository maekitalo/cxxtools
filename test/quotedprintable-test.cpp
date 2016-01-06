/*
 * Copyright (C) 2013 Tommi Maekitalo
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
#include "cxxtools/quotedprintablecodec.h"
#include "cxxtools/quotedprintablestream.h"

class QuotedPrintableTest : public cxxtools::unit::TestSuite
{
    public:
        QuotedPrintableTest()
            : cxxtools::unit::TestSuite("quotedprintable")
        {
            registerMethod("encode", *this, &QuotedPrintableTest::encode);
            registerMethod("decode", *this, &QuotedPrintableTest::decode);
            registerMethod("stream", *this, &QuotedPrintableTest::stream);
        }

        void encode()
        {
            std::string s = "H\xe4tten H\xfcte ein \xdf im Namen, w\xe4ren sie m\xfcglicherweise keine H\xfcte mehr, sondern H\xfc\xdf""e.";
            std::string q = "H=E4tten H=FCte ein =DF im Namen, w=E4ren sie m=FCglicherweise keine H=FCte m=\r\nehr, sondern H=FC=DFe.";
            std::string enc = cxxtools::encode<cxxtools::QuotedPrintableCodec>(s);
            CXXTOOLS_UNIT_ASSERT_EQUALS(enc, q);
        }

        void decode()
        {
            std::string s = "H\xe4tten H\xfcte ein \xdf im Namen, w\xe4ren sie m\xfcglicherweise keine H\xfcte mehr, sondern H\xfc\xdf""e.";
            std::string q = "H=E4tten H=FCte ein =DF im Namen, w=E4ren sie m=FCglicherweise keine H=FCte m=\r\nehr, sondern H=FC=DFe.";
            std::string dec = cxxtools::decode<cxxtools::QuotedPrintableCodec>(q);
            CXXTOOLS_UNIT_ASSERT_EQUALS(dec, s);
        }

        void stream()
        {
            std::ostringstream s;
            cxxtools::QuotedPrintable_ostream q(s);
            q << "H\xe4tten H\xfcte ein \xdf im Namen, w\xe4ren sie m\xf6glicherweise keine H\xfcte mehr,\nsondern H\xfc\xdf" "e.";

            std::string qq = s.str();

            CXXTOOLS_UNIT_ASSERT_EQUALS(qq,
                "H=E4tten H=FCte ein =DF im Namen, w=E4ren sie m=F6glicherweise keine H=FCte=\n"
                " mehr,\n"
                "sondern H=FC=DFe.");
        }

};

cxxtools::unit::RegisterTest<QuotedPrintableTest> register_QuotedPrintableTest;
