/*
 * Copyright (C) 2011 Tommi Maekitalo
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
#include "cxxtools/md5stream.h"
#include "cxxtools/md5.h"
#include "cxxtools/hmac.h"

class MD5Test : public cxxtools::unit::TestSuite
{

    public:
        MD5Test()
        : cxxtools::unit::TestSuite("md5")
        {
            registerMethod("testMD5", *this, &MD5Test::testMD5);
            registerMethod("testMD5stream", *this, &MD5Test::testMD5stream);
            registerMethod("testHMAC_MD5", *this, &MD5Test::testHMAC_MD5);
        }

        void testMD5stream()
        {
            cxxtools::Md5stream md5;
            std::string hexDigest;

            hexDigest = md5.getHexDigest();
            CXXTOOLS_UNIT_ASSERT_EQUALS(hexDigest, "d41d8cd98f00b204e9800998ecf8427e");

            md5 << "The quick brown fox jumps over the lazy dog.";
            hexDigest = md5.getHexDigest();
            CXXTOOLS_UNIT_ASSERT_EQUALS(hexDigest, "e4d909c290d0fb1ca068ffaddf22cbd0");

        }

        void testMD5()
        {
            std::string hexDigest;

            hexDigest = cxxtools::md5("");
            CXXTOOLS_UNIT_ASSERT_EQUALS(hexDigest, "d41d8cd98f00b204e9800998ecf8427e");

            hexDigest = cxxtools::md5("The quick brown fox jumps over the lazy dog.");
            CXXTOOLS_UNIT_ASSERT_EQUALS(hexDigest, "e4d909c290d0fb1ca068ffaddf22cbd0");
        }

        void testHMAC_MD5()
        {
            std::string hmac;

            hmac = cxxtools::hmac<cxxtools::md5_hash<std::string> >("", "");
            CXXTOOLS_UNIT_ASSERT_EQUALS(hmac, "74e6f7298a9c2d168935f58c001bad88");

            hmac = cxxtools::hmac<cxxtools::md5_hash<std::string> >("key", "The quick brown fox jumps over the lazy dog");
            CXXTOOLS_UNIT_ASSERT_EQUALS(hmac, "80070713463e7749b90c2dc24911e275");

        }
};

cxxtools::unit::RegisterTest<MD5Test> register_MD5Test;
