/*
 * Copyright (C) 2019 Tommi Maekitalo
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

#include "cxxtools/win1252codec.h"
#include "cxxtools/unit/testsuite.h"
#include "cxxtools/unit/registertest.h"
#include "cxxtools/string.h"

class Win1252Test : public cxxtools::unit::TestSuite
{

  public:
    Win1252Test()
    : cxxtools::unit::TestSuite("win1252")
    {
      registerMethod("encode", *this, &Win1252Test::encodeTest);
      registerMethod("decode", *this, &Win1252Test::decodeTest);
    }

    void encodeTest()
    {
      cxxtools::String ustr(L"Hi \xe4 there, 5 \u20ac");
      std::string bstr = cxxtools::Win1252Codec::encode(ustr);
      CXXTOOLS_UNIT_ASSERT_EQUALS(bstr, "Hi \xe4 there, 5 \x80");
    }

    void decodeTest()
    {
      std::string bstr("\xe4\xf6\xfc\xdf\x80");
      cxxtools::String ustr = cxxtools::Win1252Codec::decode(bstr);
      CXXTOOLS_UNIT_ASSERT(ustr == L"\xe4\xf6\xfc\xdf\u20ac");
    }

};

cxxtools::unit::RegisterTest<Win1252Test> register_Win1252Test;
