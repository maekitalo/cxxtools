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
#include "cxxtools/xml/xmlreader.h"
#include "cxxtools/xml/startelement.h"
#include "cxxtools/xml/entityresolver.h"
#include "cxxtools/unit/testsuite.h"
#include "cxxtools/unit/registertest.h"

class XmlReaderTest : public cxxtools::unit::TestSuite
{
    public:
        XmlReaderTest()
        : cxxtools::unit::TestSuite("xmlreader")
        {
            registerMethod("XmlReadXml", *this, &XmlReaderTest::XmlReadXml);
            registerMethod("XmlReadEmptyXml", *this, &XmlReaderTest::XmlReadEmptyXml);
            registerMethod("XmlReadAttributes", *this, &XmlReaderTest::XmlReadAttributes);
            registerMethod("XmlReadAttributesFromEmptyXml", *this, &XmlReaderTest::XmlReadAttributesFromEmptyXml);
            registerMethod("XmlEntity", *this, &XmlReaderTest::XmlEntity);
            registerMethod("ReverseEntity", *this, &XmlReaderTest::ReverseEntity);
            registerMethod("AllEntities", *this, &XmlReaderTest::AllEntities);
        }

        void setUp()
        {
        }

        void tearDown()
        {
        }

        void XmlReadXml()
        {
            std::istringstream in(
                "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                "<root><foo></foo></root>");
            cxxtools::xml::XmlReader xr(in);

            CXXTOOLS_UNIT_ASSERT_EQUALS(xr.documentVersion().narrow(), "1.0");
            CXXTOOLS_UNIT_ASSERT_EQUALS(xr.documentEncoding().narrow(), "UTF-8");

            cxxtools::xml::StartElement root = xr.nextElement();
            cxxtools::xml::StartElement foo = xr.nextElement();

            CXXTOOLS_UNIT_ASSERT_EQUALS(root.name().narrow(), "root");
            CXXTOOLS_UNIT_ASSERT_EQUALS(foo.name().narrow(), "foo");
        }

        void XmlReadEmptyXml()
        {
            std::istringstream in(
                "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                "<root/>");
            cxxtools::xml::XmlReader xr(in);

            CXXTOOLS_UNIT_ASSERT_EQUALS(xr.documentVersion().narrow(), "1.0");
            CXXTOOLS_UNIT_ASSERT_EQUALS(xr.documentEncoding().narrow(), "UTF-8");

            cxxtools::xml::StartElement root = xr.nextElement();
            CXXTOOLS_UNIT_ASSERT_EQUALS(root.name().narrow(), "root");
        }

        void XmlReadAttributes()
        {
            std::istringstream in(
                "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                "<root attr1=\"one\" attr2=\"two\" ><foo fooattr=\"bar\"/></root>");
            cxxtools::xml::XmlReader xr(in);

            CXXTOOLS_UNIT_ASSERT_EQUALS(xr.documentVersion().narrow(), "1.0");
            CXXTOOLS_UNIT_ASSERT_EQUALS(xr.documentEncoding().narrow(), "UTF-8");

            cxxtools::xml::StartElement root = xr.nextElement();
            cxxtools::xml::StartElement foo = xr.nextElement();

            CXXTOOLS_UNIT_ASSERT_EQUALS(root.attributes().size(), 2);
            CXXTOOLS_UNIT_ASSERT_EQUALS(root.attribute(L"attr1").narrow(), "one");
            CXXTOOLS_UNIT_ASSERT_EQUALS(root.attribute(L"attr2").narrow(), "two");
            CXXTOOLS_UNIT_ASSERT_EQUALS(foo.attributes().size(), 1);
            CXXTOOLS_UNIT_ASSERT_EQUALS(foo.attribute(L"fooattr").narrow(), "bar");
        }

        void XmlReadAttributesFromEmptyXml()
        {
            std::istringstream in(
                "<root attr1=\"one\" attr2=\"two\"/>");
            cxxtools::xml::XmlReader xr(in);

            cxxtools::xml::StartElement root = xr.nextElement();

            CXXTOOLS_UNIT_ASSERT_EQUALS(root.attributes().size(), 2);
            CXXTOOLS_UNIT_ASSERT_EQUALS(root.attribute(L"attr1").narrow(), "one");
            CXXTOOLS_UNIT_ASSERT_EQUALS(root.attribute(L"attr2").narrow(), "two");
        }

        void XmlEntity()
        {
            cxxtools::xml::EntityResolver resolver;
            cxxtools::String r;

            r = resolver.resolveEntity(cxxtools::String(L"auml"));
            CXXTOOLS_UNIT_ASSERT(r == cxxtools::String(1, cxxtools::Char(0xE4)));

            r = resolver.resolveEntity(cxxtools::String(L"Ouml"));
            CXXTOOLS_UNIT_ASSERT(r == cxxtools::String(1, cxxtools::Char(0xD6)));

            r = resolver.resolveEntity(cxxtools::String(L"AElig"));
            CXXTOOLS_UNIT_ASSERT(r == cxxtools::String(1, cxxtools::Char(0xC6)));

            r = resolver.resolveEntity(cxxtools::String(L"zwnj"));
            CXXTOOLS_UNIT_ASSERT(r == cxxtools::String(1, cxxtools::Char(0x200C)));

            r = resolver.resolveEntity(cxxtools::String(L"#x200C"));
            CXXTOOLS_UNIT_ASSERT(r == cxxtools::String(1, cxxtools::Char(0x200C)));

            r = resolver.resolveEntity(cxxtools::String(L"#1234"));
            CXXTOOLS_UNIT_ASSERT(r == cxxtools::String(1, cxxtools::Char(1234)));

            CXXTOOLS_UNIT_ASSERT_THROW(resolver.resolveEntity(cxxtools::String(L"zwnjj")), std::exception);
            CXXTOOLS_UNIT_ASSERT_THROW(resolver.resolveEntity(cxxtools::String(L"AEli")), std::exception);

        }

        void ReverseEntity()
        {
            cxxtools::xml::EntityResolver resolver;

            cxxtools::String r;

            r = resolver.getEntity(cxxtools::Char(L'&'));
            CXXTOOLS_UNIT_ASSERT_MSG(r == cxxtools::String(L"&amp;"),
                "failed to get entity for character '&'; expected \"&amp;\" returned \"" << r.narrow() << '"');

            r = resolver.getEntity(cxxtools::Char(0x0022));
            CXXTOOLS_UNIT_ASSERT_MSG(r == cxxtools::String(L"&quot;"),
                "failed to get entity for character code 0x0022; expected \"&quot;\" returned \"" << r.narrow() << '"');

            r = resolver.getEntity(cxxtools::Char(1234));
            CXXTOOLS_UNIT_ASSERT_MSG(r == cxxtools::String(L"&#1234;"),
                "failed to get entity for character code 1234; expected \"&#1234;\" returned \"" << r.narrow() << '"');

        }

        void AllEntities()
        {
            cxxtools::xml::EntityResolver resolver;
            for (cxxtools::Char::value_type n = 0; n <= 0xFFFF; ++n)
            {
                cxxtools::String r = resolver.getEntity(cxxtools::Char(n));
                if (r.size() > 2 && r[0] == '&' && r[r.size() - 1] == ';')
                {
                    cxxtools::String rr = resolver.resolveEntity(r.substr(1, r.size() - 2));
                    CXXTOOLS_UNIT_ASSERT_MSG(
                        rr == cxxtools::String(1, cxxtools::Char(n)),
                        "resolving char code " << n << " failed; entity \"" << r.narrow() << '"');
                }
                else
                {
                    CXXTOOLS_UNIT_ASSERT_MSG(r[0] == cxxtools::Char(n),
                        "resolving char code " << n << " failed");
                }
            }
        }

};

cxxtools::unit::RegisterTest<XmlReaderTest> register_XmlReaderTest;
