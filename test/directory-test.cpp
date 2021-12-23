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
        registerMethod("updir", *this, &DirectoryTest::updir);
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

    void updir()
    {
        cxxtools::Directory libdir("/usr/lib");

        cxxtools::Directory usrdir = libdir.updir();

        CXXTOOLS_UNIT_ASSERT(!usrdir.isRoot());
        CXXTOOLS_UNIT_ASSERT_EQUALS(usrdir.path(), "/usr");

        cxxtools::Directory rootdir = usrdir.updir();
        CXXTOOLS_UNIT_ASSERT(rootdir.isRoot());
        CXXTOOLS_UNIT_ASSERT_EQUALS(rootdir.path(), "/");
    }
};

cxxtools::unit::RegisterTest<DirectoryTest> register_DirectoryTest;
