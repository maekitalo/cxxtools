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

#include "cxxtools/utf8codec.h"
#include "cxxtools/utf8.h"
#include "cxxtools/textstream.h"
#include "cxxtools/unit/testsuite.h"
#include "cxxtools/unit/registertest.h"
#include "cxxtools/string.h"
#include <sstream>
#include <iomanip>

#define SIZEOF(a)  (sizeof(a) / sizeof(a[0]))

class Utf8Test : public cxxtools::unit::TestSuite
{

  public:
    Utf8Test()
    : cxxtools::unit::TestSuite("utf8")
    {
      registerMethod("encode", *this, &Utf8Test::encodeTest);
      registerMethod("decode", *this, &Utf8Test::decodeTest);
      registerMethod("byteordermark", *this, &Utf8Test::byteordermarkTest);
      registerMethod("incompleteBom", *this, &Utf8Test::incompleteBomTest);
      registerMethod("partialBom", *this, &Utf8Test::partialBomTest);
      registerMethod("consumeInput", *this, &Utf8Test::consumeInput);
      registerMethod("fillOutput", *this, &Utf8Test::fillOutput);
      registerMethod("partialDecode", *this, &Utf8Test::partialDecode);
      registerMethod("istream", *this, &Utf8Test::istream);
      registerMethod("ostream", *this, &Utf8Test::ostream);
    }

    void encodeTest()
    {
      cxxtools::String ustr(L"Hi \xe4 there");
      std::string bstr = cxxtools::Utf8Codec::encode(ustr);
      CXXTOOLS_UNIT_ASSERT_EQUALS(bstr, "Hi \xc3\xa4 there");

      std::string bstr2 = cxxtools::Utf8(ustr);
      CXXTOOLS_UNIT_ASSERT_EQUALS(bstr2, "Hi \xc3\xa4 there");
    }

    void decodeTest()
    {
      std::string bstr("Hi \xc3\xa4 there");
      cxxtools::String ustr = cxxtools::Utf8Codec::decode(bstr);
      CXXTOOLS_UNIT_ASSERT(ustr == L"Hi \xe4 there");
    }

    void byteordermarkTest()
    {
      std::string bstr("\xef\xbb\xbfHi \xc3\xa4 there");
      cxxtools::String ustr = cxxtools::Utf8Codec::decode(bstr);
      CXXTOOLS_UNIT_ASSERT(ustr == L"Hi \xe4 there");
    }

    void incompleteBomTest()
    {
      std::string bstr("\xef\xbb");
      cxxtools::String ustr = cxxtools::Utf8Codec::decode(bstr);
      CXXTOOLS_UNIT_ASSERT(ustr.empty());
    }

    void partialBomTest()
    {
      // check whether codec is able to partially consume byte order mark
      cxxtools::Utf8Codec codec;
      const char data[] = { '\xef', '\xbb', '\xbf', 'A' };
      cxxtools::Char to[10];
      const char* fromBegin = data;
      const char* fromNext = data;
      cxxtools::Char* toNext = to;
      cxxtools::MBState mbstate;

      // consume first char
      codec.in(mbstate, fromBegin, fromBegin + 1, fromNext, to, to + 10, toNext);
      CXXTOOLS_UNIT_ASSERT(fromNext > fromBegin);  // codec must consume char
      CXXTOOLS_UNIT_ASSERT_EQUALS(toNext - to, 0);   // no output yet

      fromBegin = fromNext;

      codec.in(mbstate, fromBegin, fromBegin + 1, fromNext, to, to + 10, toNext);
      CXXTOOLS_UNIT_ASSERT(fromNext > fromBegin);  // codec must consume another char
      CXXTOOLS_UNIT_ASSERT_EQUALS(toNext - to, 0);   // no output yet

      fromBegin = fromNext;

      codec.in(mbstate, fromBegin, fromBegin + 2, fromNext, to, to + 10, toNext);
      CXXTOOLS_UNIT_ASSERT(fromNext > fromBegin);  // codec must consume the third and forth char
      CXXTOOLS_UNIT_ASSERT_EQUALS(toNext - to, 1);   // now output
      CXXTOOLS_UNIT_ASSERT_EQUALS(to[0].narrow(), 'A');   // now output
    }

    void consumeInput()
    {
      const char utf8Input[] = "Hello \xc3\xa4\xe2\x80\x93 end";
      cxxtools::Char unicodeOutput[32];

      cxxtools::Utf8Codec codec;
      cxxtools::MBState mbstate;
      const char* fromBegin = utf8Input;
      const char* fromNext = utf8Input;
      const char* fromEnd = utf8Input + sizeof(utf8Input) - 1;
      cxxtools::Char* toNext = unicodeOutput;
      cxxtools::Char* toEnd = unicodeOutput + SIZEOF(unicodeOutput);

      codec.in(mbstate, fromBegin, fromEnd, fromNext, unicodeOutput, toEnd, toNext);

      // is input completely consumed
      CXXTOOLS_UNIT_ASSERT_EQUALS(fromNext, fromBegin + sizeof(utf8Input) - 1);

      // is output completely converted
      CXXTOOLS_UNIT_ASSERT_EQUALS(toNext - unicodeOutput, 12);

      // check some values
      CXXTOOLS_UNIT_ASSERT_EQUALS(unicodeOutput[4].value(), 0x6f);
      CXXTOOLS_UNIT_ASSERT_EQUALS(unicodeOutput[6].value(), 0xe4);
      CXXTOOLS_UNIT_ASSERT_EQUALS(unicodeOutput[7].value(), 0x2013);
    }

    void fillOutput()
    {
      const char utf8Input[] = "Hello \xc3\xa4\xe2\x80\x93 end";
      cxxtools::Char unicodeOutput[8];

      cxxtools::Utf8Codec codec;
      cxxtools::MBState mbstate;
      const char* fromBegin = utf8Input;
      const char* fromNext = utf8Input;
      const char* fromEnd = utf8Input + sizeof(utf8Input) - 1;
      cxxtools::Char* toNext = unicodeOutput;
      cxxtools::Char* toEnd = unicodeOutput + SIZEOF(unicodeOutput);

      codec.in(mbstate, fromBegin, fromEnd, fromNext, unicodeOutput, toEnd, toNext);

      // is output filled
      CXXTOOLS_UNIT_ASSERT_EQUALS(toNext - unicodeOutput, 8);

      // check some values
      CXXTOOLS_UNIT_ASSERT_EQUALS(unicodeOutput[4].value(), 0x6f);
      CXXTOOLS_UNIT_ASSERT_EQUALS(unicodeOutput[6].value(), 0xe4);

      // continue
      fromBegin = fromNext;
      toNext = unicodeOutput;
      codec.in(mbstate, fromBegin, fromEnd, fromNext, unicodeOutput, unicodeOutput + SIZEOF(unicodeOutput), toNext);

      // got the rest?
      CXXTOOLS_UNIT_ASSERT_EQUALS(toNext - unicodeOutput, 4);

      // input consumed?
      CXXTOOLS_UNIT_ASSERT_EQUALS(fromNext, fromEnd);
    }

    void partialDecode()
    {
      const char utf8Input[] = "Hello \xc3\xa4\xe2\x80\x93 end";
      cxxtools::Char unicodeOutput[32];

      cxxtools::Utf8Codec codec;
      cxxtools::MBState mbstate;
      const char* fromBegin = utf8Input;
      const char* fromNext = utf8Input;
      const char* fromEnd = utf8Input + sizeof(utf8Input) - 1;
      cxxtools::Char* toNext = unicodeOutput;
      cxxtools::Char* toEnd = unicodeOutput + SIZEOF(unicodeOutput);

      cxxtools::Utf8Codec::result result = codec.in(mbstate, fromBegin, fromBegin + 7, fromNext, unicodeOutput, toEnd, toNext);
      CXXTOOLS_UNIT_ASSERT_EQUALS(result, std::codecvt_base::partial);

      result = codec.in(mbstate, fromNext, fromEnd, fromNext, toNext, toEnd, toNext);
      CXXTOOLS_UNIT_ASSERT_EQUALS(result, std::codecvt_base::ok);

      CXXTOOLS_UNIT_ASSERT_EQUALS(fromNext, fromEnd);
      CXXTOOLS_UNIT_ASSERT_EQUALS(toNext - unicodeOutput, 12);
      CXXTOOLS_UNIT_ASSERT_EQUALS(unicodeOutput[4].value(), 0x6f);
      CXXTOOLS_UNIT_ASSERT_EQUALS(unicodeOutput[6].value(), 0xe4);
      CXXTOOLS_UNIT_ASSERT_EQUALS(unicodeOutput[7].value(), 0x2013);
    }

    void istream()
    {
      std::istringstream in("Hello \xc3\xa4\xe2\x80\x93 end");
      cxxtools::TextIStream tin(in, new cxxtools::Utf8Codec());
      cxxtools::String s;
      std::getline(tin, s);

      CXXTOOLS_UNIT_ASSERT_EQUALS(s.size(), 12);
      CXXTOOLS_UNIT_ASSERT_EQUALS(s[4].value(), 0x6f);
      CXXTOOLS_UNIT_ASSERT_EQUALS(s[6].value(), 0xe4);
      CXXTOOLS_UNIT_ASSERT_EQUALS(s[7].value(), 0x2013);
    }

    void ostream()
    {
      std::ostringstream out;
      cxxtools::TextOStream tout(out, new cxxtools::Utf8Codec());
      tout << L"Hello \x00e4\x2013 end" << std::flush;
      std::string s = out.str();
      CXXTOOLS_UNIT_ASSERT_EQUALS(s.size(), 15);
      CXXTOOLS_UNIT_ASSERT_EQUALS(s, "Hello \xc3\xa4\xe2\x80\x93 end");
    }
};

cxxtools::unit::RegisterTest<Utf8Test> register_Utf8Test;
