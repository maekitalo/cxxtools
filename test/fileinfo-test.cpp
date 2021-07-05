#include <cxxtools/fileinfo.h>
#include <cxxtools/unit/testsuite.h>
#include <cxxtools/unit/registertest.h>
#include <fstream>

class FileInfoTest : public cxxtools::unit::TestSuite
{
public:
    FileInfoTest()
    : cxxtools::unit::TestSuite("fileinfo")
    {
        registerMethod("createRemove", *this, &FileInfoTest::createRemove);
        registerMethod("path", *this, &FileInfoTest::path);
    }

    void createRemove()
    {
        static const char testfileName[] = "cxxtools-testfile.txt";

        {
            cxxtools::FileInfo fi(testfileName);

            CXXTOOLS_UNIT_ASSERT(!fi.exists());
            CXXTOOLS_UNIT_ASSERT_EQUALS(fi.path(), testfileName);
            CXXTOOLS_UNIT_ASSERT_EQUALS(fi.name(), testfileName);
            CXXTOOLS_UNIT_ASSERT_EQUALS(fi.type(), cxxtools::FileInfo::Invalid);
        }

        {
            std::ofstream f(testfileName);
            cxxtools::FileInfo fi(testfileName);

            CXXTOOLS_UNIT_ASSERT(fi.exists());
            CXXTOOLS_UNIT_ASSERT_EQUALS(fi.type(), cxxtools::FileInfo::File);
            CXXTOOLS_UNIT_ASSERT(fi.isFile());

            fi.remove();
            CXXTOOLS_UNIT_ASSERT(!fi.exists());
        }

        {
            cxxtools::FileInfo fi(testfileName);
            CXXTOOLS_UNIT_ASSERT(!fi.exists());
        }
    }

    void path()
    {
        {
            cxxtools::FileInfo foo("foo");
            cxxtools::FileInfo bar("bar");
            cxxtools::FileInfo foobar = foo + bar;
            CXXTOOLS_UNIT_ASSERT_EQUALS(foobar.path(), "foo/bar");
        }
        
        {
            cxxtools::FileInfo foo("foo/");
            cxxtools::FileInfo bar("bar");
            cxxtools::FileInfo foobar = foo + bar;
            CXXTOOLS_UNIT_ASSERT_EQUALS(foobar.path(), "foo/bar");
        }
        
        {
            cxxtools::FileInfo foo("foo");
            cxxtools::FileInfo bar("/bar");
            cxxtools::FileInfo foobar = foo + bar;
            CXXTOOLS_UNIT_ASSERT_EQUALS(foobar.path(), "foo/bar");
        }
        
        {
            cxxtools::FileInfo foo("foo/");
            cxxtools::FileInfo bar("/bar");
            cxxtools::FileInfo foobar = foo + bar;
            CXXTOOLS_UNIT_ASSERT_EQUALS(foobar.path(), "foo/bar");
        }
    }
};

cxxtools::unit::RegisterTest<FileInfoTest> register_FileInfoTest;
