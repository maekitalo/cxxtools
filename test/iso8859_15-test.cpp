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

#include "cxxtools/iso8859_15codec.h"
#include "cxxtools/unit/testsuite.h"
#include "cxxtools/unit/registertest.h"
#include "cxxtools/string.h"

class Iso8859_15Test : public cxxtools::unit::TestSuite
{

  public:
    Iso8859_15Test()
    : cxxtools::unit::TestSuite("iso8859_15")
    {
      registerMethod("encode", *this, &Iso8859_15Test::encodeTest);
      registerMethod("decode", *this, &Iso8859_15Test::decodeTest);
    }

    void encodeTest()
    {
      cxxtools::String ustr(L"Hi \xe4 there, 5 \x20ac");
      std::string bstr = cxxtools::Iso8859_15Codec::encode(ustr);
      CXXTOOLS_UNIT_ASSERT_EQUALS(bstr, "Hi \xe4 there, 5 \xa4");
    }

    void decodeTest()
    {
      std::string bstr("Hi \xe4 there");
      cxxtools::String ustr = cxxtools::Iso8859_15Codec::decode(bstr);
      CXXTOOLS_UNIT_ASSERT(ustr == L"Hi \xe4 there");
    }

};

cxxtools::unit::RegisterTest<Iso8859_15Test> register_Iso8859_15Test;
