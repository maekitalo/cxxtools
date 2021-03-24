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
#include "cxxtools/log.h"
#include "cxxtools/file.h"
#include "cxxtools/fileinfo.h"
#include "cxxtools/ioerror.h"
#include "cxxtools/datetime.h"
#include "cxxtools/timespan.h"
#include <fstream>

log_define("cxxtools.test.file")

const std::string tmpFileName = "file-test.tmp";
const std::string tmpFileName2 = "file-test-2.tmp";

class FileTest : public cxxtools::unit::TestSuite
{
        void forceRemoveFile(const std::string& fname)
        {
            try
            {
                cxxtools::FileInfo(fname).remove();
            }
            catch (const cxxtools::FileNotFound&)
            {
            }
        }

    public:
        FileTest()
            : cxxtools::unit::TestSuite("file")
        {
            registerMethod("testCreateFile", *this, &FileTest::testCreateFile);
            registerMethod("testRemoveFile", *this, &FileTest::testRemoveFile);
            registerMethod("testMoveFile", *this, &FileTest::testMoveFile);
            registerMethod("testLinkFile", *this, &FileTest::testLinkFile);
            registerMethod("testSymlinkFile", *this, &FileTest::testSymlinkFile);
            registerMethod("testCopyFile", *this, &FileTest::testCopyFile);
            registerMethod("testFilesize", *this, &FileTest::testFilesize);
            registerMethod("testCopyBigFile", *this, &FileTest::testCopyBigFile);
            registerMethod("testMTime", *this, &FileTest::testMTime);
        }

        void setUp()
        {
            forceRemoveFile(tmpFileName);
            forceRemoveFile(tmpFileName2);
        }

        void tearDown()
        {
            forceRemoveFile(tmpFileName);
            forceRemoveFile(tmpFileName2);
        }

        void testCreateFile()
        {
            CXXTOOLS_UNIT_ASSERT(!cxxtools::FileInfo::exists(tmpFileName));

            cxxtools::File::create(tmpFileName);

            CXXTOOLS_UNIT_ASSERT(cxxtools::FileInfo::exists(tmpFileName));
        }

        void testRemoveFile()
        {
            cxxtools::File::create(tmpFileName);
            cxxtools::File f(tmpFileName);
            f.remove();

            CXXTOOLS_UNIT_ASSERT(!cxxtools::FileInfo::exists(tmpFileName));
        }

        void testMoveFile()
        {
            cxxtools::File::create(tmpFileName);
            cxxtools::File f(tmpFileName);
            f.move(tmpFileName2);

            CXXTOOLS_UNIT_ASSERT(!cxxtools::FileInfo::exists(tmpFileName));
            CXXTOOLS_UNIT_ASSERT(cxxtools::FileInfo::exists(tmpFileName2));
        }

        void testLinkFile()
        {
            cxxtools::File::create(tmpFileName);
            cxxtools::File f(tmpFileName);
            f.link(tmpFileName2);

            cxxtools::FileInfo fi(tmpFileName);
            cxxtools::FileInfo fi2(tmpFileName2);

            CXXTOOLS_UNIT_ASSERT_EQUALS(fi.type(), cxxtools::FileInfo::File);
            CXXTOOLS_UNIT_ASSERT_EQUALS(fi2.type(), cxxtools::FileInfo::File);
        }

        void testSymlinkFile()
        {
            cxxtools::File::create(tmpFileName);
            cxxtools::File f(tmpFileName);
            f.symlink(tmpFileName2);

            cxxtools::FileInfo fi(tmpFileName);
            cxxtools::FileInfo fi2(tmpFileName2);

            CXXTOOLS_UNIT_ASSERT_EQUALS(fi.type(), cxxtools::FileInfo::File);
            CXXTOOLS_UNIT_ASSERT_EQUALS(fi2.type(), cxxtools::FileInfo::Symlink);
        }

        void testCopyFile()
        {
            cxxtools::File::create(tmpFileName);
            cxxtools::File f(tmpFileName);
            f.copy(tmpFileName2);

            cxxtools::FileInfo fi(tmpFileName);
            cxxtools::FileInfo fi2(tmpFileName2);

            CXXTOOLS_UNIT_ASSERT_EQUALS(fi.type(), cxxtools::FileInfo::File);
            CXXTOOLS_UNIT_ASSERT_EQUALS(fi2.type(), cxxtools::FileInfo::File);
        }

        void testFilesize()
        {
            std::string data;
            for (unsigned n = 0; n < 0x10003; ++n)
              data += static_cast<char>(n);

            std::ofstream f(tmpFileName.c_str());
            f << data;
            f.close();

            cxxtools::FileInfo fi(tmpFileName);
            CXXTOOLS_UNIT_ASSERT_EQUALS(fi.size(), 0x10003);
        }

        void testCopyBigFile()
        {
            std::string data;
            for (unsigned n = 0; n < 0x10003; ++n)
              data += static_cast<char>(n);

            std::ofstream of(tmpFileName.c_str());
            of << data;
            of.close();

            cxxtools::File f(tmpFileName);
            f.copy(tmpFileName2);

            cxxtools::FileInfo fi2(tmpFileName2);
            CXXTOOLS_UNIT_ASSERT_EQUALS(fi2.size(), 0x10003);

            std::ostringstream s;
            std::ifstream ifs(tmpFileName.c_str());
            s << ifs.rdbuf();

            CXXTOOLS_UNIT_ASSERT_EQUALS(s.str().size(), 0x10003);
            CXXTOOLS_UNIT_ASSERT(s.str() == data);
        }

        void testMTime()
        {
            std::ofstream f(tmpFileName.c_str());
            f.close();

            cxxtools::File fi(tmpFileName);

            cxxtools::UtcDateTime mtime = fi.mtime();
            cxxtools::UtcDateTime ctime = fi.ctime();
            cxxtools::UtcDateTime now = cxxtools::DateTime::gmtime();

            CXXTOOLS_UNIT_ASSERT(mtime <= now);
            CXXTOOLS_UNIT_ASSERT(mtime >= now - cxxtools::Minutes(1));
            CXXTOOLS_UNIT_ASSERT(ctime <= now);
            CXXTOOLS_UNIT_ASSERT(ctime >= now - cxxtools::Minutes(1));
        }

};

cxxtools::unit::RegisterTest<FileTest> register_FileTest;
