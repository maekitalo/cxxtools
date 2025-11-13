#include <cxxtools/unit/testsuite.h>
#include <cxxtools/unit/registertest.h>
#include <cxxtools/bufferedreader.h>
#include <cxxtools/filedevice.h>
#include <cxxtools/fileinfo.h>
#include <cxxtools/ioerror.h>
#include <cxxtools/log.h>
#include <fstream>

log_define("cxxtools.unit.bufferedreader")

static const std::string tmpFileName = "file-test.tmp";

class BufferedReaderTest : public cxxtools::unit::TestSuite
{
    void createFile(unsigned size)
    {
        std::ofstream file(tmpFileName);
        for (unsigned n = 0; n < size; ++n)
            file << static_cast<char>('A' + (n % 26));
    }

public:
    BufferedReaderTest()
        : cxxtools::unit::TestSuite("bufferedreader")
    {
        registerMethod("get", *this, &BufferedReaderTest::get);
        registerMethod("peek", *this, &BufferedReaderTest::peek);
        registerMethod("read", *this, &BufferedReaderTest::read);
    }

    void tearDown() override
    {
        try
        {
            cxxtools::FileInfo(tmpFileName).remove();
        }
        catch (const cxxtools::FileNotFound&)
        {
        }
    }

    void get();
    void peek();
    void read();
};

void BufferedReaderTest::get()
{
    createFile(4);
    cxxtools::FileDevice fileDevice(tmpFileName, cxxtools::IOS_OpenMode::IOS_Read, false);
    cxxtools::BufferedReader reader(fileDevice, 2);
    char ch;

    ch = reader.get();
    CXXTOOLS_UNIT_ASSERT(!reader.eof());
    CXXTOOLS_UNIT_ASSERT_EQUALS(ch, 'A');

    ch = reader.get();
    CXXTOOLS_UNIT_ASSERT(!reader.eof());
    CXXTOOLS_UNIT_ASSERT_EQUALS(ch, 'B');

    ch = reader.get();
    CXXTOOLS_UNIT_ASSERT(!reader.eof());
    CXXTOOLS_UNIT_ASSERT_EQUALS(ch, 'C');

    ch = reader.get();
    CXXTOOLS_UNIT_ASSERT(!reader.eof());
    CXXTOOLS_UNIT_ASSERT_EQUALS(ch, 'D');

    reader.get();
    CXXTOOLS_UNIT_ASSERT(reader.eof());
}

void BufferedReaderTest::peek()
{
    createFile(4);
    cxxtools::FileDevice fileDevice(tmpFileName, cxxtools::IOS_OpenMode::IOS_Read, false);
    cxxtools::BufferedReader reader(fileDevice, 2);
    char ch;

    ch = reader.peek();
    CXXTOOLS_UNIT_ASSERT(!reader.eof());
    CXXTOOLS_UNIT_ASSERT_EQUALS(ch, 'A');

    ch = reader.peek();
    CXXTOOLS_UNIT_ASSERT(!reader.eof());
    CXXTOOLS_UNIT_ASSERT_EQUALS(ch, 'A');

    reader.get();
    ch = reader.peek();
    CXXTOOLS_UNIT_ASSERT(!reader.eof());
    CXXTOOLS_UNIT_ASSERT_EQUALS(ch, 'B');

    reader.get();
    ch = reader.peek();
    CXXTOOLS_UNIT_ASSERT(!reader.eof());
    CXXTOOLS_UNIT_ASSERT_EQUALS(ch, 'C');

    reader.get();
    ch = reader.peek();
    CXXTOOLS_UNIT_ASSERT(!reader.eof());
    CXXTOOLS_UNIT_ASSERT_EQUALS(ch, 'D');

    reader.get();
    reader.peek();
    CXXTOOLS_UNIT_ASSERT(reader.eof());
}

void BufferedReaderTest::read()
{
    createFile(220);
    cxxtools::FileDevice fileDevice(tmpFileName, cxxtools::IOS_OpenMode::IOS_Read, false);
    cxxtools::BufferedReader reader(fileDevice, 80);
    char inpBuf[27];
    unsigned count;

    for (unsigned n = 0; n < 8; ++n)
    {
        log_debug("read " << n << " in_avail " << reader.in_avail());
        count = reader.read(inpBuf, 27);
        CXXTOOLS_UNIT_ASSERT_EQUALS(count, 27);
        CXXTOOLS_UNIT_ASSERT_EQUALS(inpBuf[0], static_cast<char>('A' + n));
    }

    log_debug("read more" << " in_avail " << reader.in_avail());
    count = reader.read(inpBuf, 27);
    CXXTOOLS_UNIT_ASSERT(!reader.eof());
    CXXTOOLS_UNIT_ASSERT_EQUALS(count, 4);

    count = reader.read(inpBuf, 27);
    CXXTOOLS_UNIT_ASSERT(reader.eof());
    CXXTOOLS_UNIT_ASSERT_EQUALS(count, 0);

}

cxxtools::unit::RegisterTest<BufferedReaderTest> register_BufferedReaderTest;
