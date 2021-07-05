#include <cxxtools/directory.h>
#include <cxxtools/unit/testsuite.h>
#include <cxxtools/unit/registertest.h>

class DirectoryTest : public cxxtools::unit::TestSuite
{
public:
    DirectoryTest()
    : cxxtools::unit::TestSuite("directory")
    {
        registerMethod("createDirectory", *this, &DirectoryTest::createDirectory);
    }

    void createDirectory()
    {
        CXXTOOLS_UNIT_ASSERT(!cxxtools::Directory::exists("foo"));

        cxxtools::Directory::create("foo/bar/baz", true);
        CXXTOOLS_UNIT_ASSERT(cxxtools::Directory::exists("foo/bar/baz"));

        cxxtools::Directory("foo/bar/baz").remove();
        CXXTOOLS_UNIT_ASSERT(!cxxtools::Directory::exists("foo/bar/baz"));
        CXXTOOLS_UNIT_ASSERT(cxxtools::Directory::exists("foo/bar"));

        cxxtools::Directory("foo/bar").remove();
        CXXTOOLS_UNIT_ASSERT(!cxxtools::Directory::exists("foo/bar"));
        CXXTOOLS_UNIT_ASSERT(cxxtools::Directory::exists("foo"));

        cxxtools::Directory("foo").remove();
        CXXTOOLS_UNIT_ASSERT(!cxxtools::Directory::exists("foo"));
    }
};

cxxtools::unit::RegisterTest<DirectoryTest> register_DirectoryTest;
