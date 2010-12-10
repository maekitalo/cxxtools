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

#include "cxxtools/net/uri.h"
#include "cxxtools/unit/testsuite.h"
#include "cxxtools/unit/registertest.h"

class UriTest : public cxxtools::unit::TestSuite
{
    public:
        UriTest()
        : cxxtools::unit::TestSuite("cxxtools-arg-Test")
        {
            registerMethod("testUri_UPHP", *this, &UriTest::testUri_UPHP);
            registerMethod("testUri_UHP", *this, &UriTest::testUri_UHP);
            registerMethod("testUri_UPH", *this, &UriTest::testUri_UPH);
            registerMethod("testUri_HP", *this, &UriTest::testUri_HP);
            registerMethod("testUri_H", *this, &UriTest::testUri_H);
            registerMethod("testUri_UPH6P", *this, &UriTest::testUri_UPH6P);
            registerMethod("testUri_UH6P", *this, &UriTest::testUri_UH6P);
            registerMethod("testUri_UPH6", *this, &UriTest::testUri_UPH6);
            registerMethod("testUri_H6P", *this, &UriTest::testUri_H6P);
            registerMethod("testUri_H6", *this, &UriTest::testUri_H6);
            registerMethod("testQuery", *this, &UriTest::testQuery);
            registerMethod("testFragment", *this, &UriTest::testFragment);
            registerMethod("testQueryFragment", *this, &UriTest::testQueryFragment);
            registerMethod("testHttpPort", *this, &UriTest::testHttpPort);
            registerMethod("testHttpsPort", *this, &UriTest::testHttpsPort);
            registerMethod("testFtpPort", *this, &UriTest::testFtpPort);
            registerMethod("testUriStr", *this, &UriTest::testUriStr);
        }

        void testUri_UPHP()
        {
            cxxtools::net::Uri uri("http://user:password@host:56/blah.html");

            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.protocol(), "http");
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.user(), "user");
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.password(), "password");
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.host(), "host");
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.port(), 56);
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.path(), "/blah.html");
        }

        void testUri_UHP()
        {
            cxxtools::net::Uri uri("http://user@host:56/blah.html");

            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.protocol(), "http");
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.user(), "user");
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.password(), "");
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.host(), "host");
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.port(), 56);
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.path(), "/blah.html");
        }

        void testUri_UPH()
        {
            cxxtools::net::Uri uri("http://user:password@host/blah.html");

            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.protocol(), "http");
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.user(), "user");
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.password(), "password");
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.host(), "host");
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.port(), 80);
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.path(), "/blah.html");
        }

        void testUri_HP()
        {
            cxxtools::net::Uri uri("http://host:56/blah.html");

            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.protocol(), "http");
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.user(), "");
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.password(), "");
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.host(), "host");
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.port(), 56);
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.path(), "/blah.html");
        }

        void testUri_H()
        {
            cxxtools::net::Uri uri("http://host/blah.html");

            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.protocol(), "http");
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.user(), "");
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.password(), "");
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.host(), "host");
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.port(), 80);
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.path(), "/blah.html");
        }

        void testUri_UPH6P()
        {
            cxxtools::net::Uri uri("http://user:password@[::1]:56/blah.html");

            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.protocol(), "http");
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.user(), "user");
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.password(), "password");
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.host(), "::1");
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.port(), 56);
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.path(), "/blah.html");
        }

        void testUri_UH6P()
        {
            cxxtools::net::Uri uri("http://user@[::1]:56/blah.html");

            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.protocol(), "http");
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.user(), "user");
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.password(), "");
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.host(), "::1");
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.port(), 56);
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.path(), "/blah.html");
        }

        void testUri_UPH6()
        {
            cxxtools::net::Uri uri("http://user:password@[::1]/blah.html");

            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.protocol(), "http");
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.user(), "user");
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.password(), "password");
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.host(), "::1");
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.port(), 80);
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.path(), "/blah.html");
        }

        void testUri_H6P()
        {
            cxxtools::net::Uri uri("http://[::1]:56/blah.html");

            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.protocol(), "http");
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.user(), "");
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.password(), "");
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.host(), "::1");
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.port(), 56);
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.path(), "/blah.html");
        }

        void testUri_H6()
        {
            cxxtools::net::Uri uri("http://[::1]/blah.html");

            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.protocol(), "http");
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.user(), "");
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.password(), "");
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.host(), "::1");
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.port(), 80);
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.path(), "/blah.html");
        }

        void testQuery()
        {
            cxxtools::net::Uri uri("http://host/?abc=1");
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.query(), "abc=1");
        }

        void testFragment()
        {
            cxxtools::net::Uri uri("http://host/#foo");
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.fragment(), "foo");
        }

        void testQueryFragment()
        {
            cxxtools::net::Uri uri("http://host/?abc=1#foo");
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.query(), "abc=1");
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.fragment(), "foo");
        }

        void testHttpPort()
        {
            cxxtools::net::Uri uri("http://host/");
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.port(), 80);
        }

        void testHttpsPort()
        {
            cxxtools::net::Uri uri("https://host/");
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.port(), 443);
        }

        void testFtpPort()
        {
            cxxtools::net::Uri uri("ftp://host/");
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.port(), 21);
        }

        void testUriStr()
        {
            cxxtools::net::Uri uri("http://user:password@host:80/blah.html");
            CXXTOOLS_UNIT_ASSERT_EQUALS(uri.str(), "http://user:password@host/blah.html");
        }

};

cxxtools::unit::RegisterTest<UriTest> register_UriTest;
