#include <cxxtools/tz.h>

#include <cxxtools/unit/testsuite.h>
#include <cxxtools/unit/registertest.h>

class TzTest : public cxxtools::unit::TestSuite
{
public:
    TzTest()
        : cxxtools::unit::TestSuite("tz")
    {
        registerMethod("local2utc", *this, &TzTest::local2utc);
        registerMethod("local2utcNonUnique", *this, &TzTest::local2utcNonUnique);
        registerMethod("utc2local", *this, &TzTest::utc2local);
        registerMethod("offset", *this, &TzTest::offset);
        registerMethod("isDst", *this, &TzTest::isDst);
    }

    void local2utc();
    void local2utcNonUnique();
    void utc2local();
    void offset();
    void isDst();
};

void TzTest::local2utc()
{
    {
        cxxtools::Tz tz("Europe/Berlin");
        cxxtools::LocalDateTime lt(2018, 11, 7, 13, 0, 5);
        cxxtools::UtcDateTime ut = tz.toUtc(lt);

        CXXTOOLS_UNIT_ASSERT_EQUALS(ut.toString(), cxxtools::UtcDateTime(2018, 11, 7, 12, 0, 5).toString());
    }

    {
        cxxtools::Tz tz("Pacific/Samoa");
        cxxtools::LocalDateTime lt(2018, 11, 7, 13, 0, 5);
        cxxtools::UtcDateTime ut = tz.toUtc(lt);

        CXXTOOLS_UNIT_ASSERT_EQUALS(ut.toString(), cxxtools::UtcDateTime(2018, 11, 8, 0, 0, 5).toString());
    }
}

void TzTest::local2utcNonUnique()
{
    cxxtools::Tz tz("Europe/Berlin");
    cxxtools::LocalDateTime lt(2018, 10, 28, 2, 30, 6);
    CXXTOOLS_UNIT_ASSERT_THROW(tz.toUtc(lt), cxxtools::TzAmbiguousLocalTime);
    CXXTOOLS_UNIT_ASSERT_NOTHROW(tz.toUtc(lt, true));
    CXXTOOLS_UNIT_ASSERT_NOTHROW(tz.toUtc(lt, false));
    CXXTOOLS_UNIT_ASSERT_EQUALS(tz.toUtc(lt, true).toString(), "2018-10-28 00:30:06");
    CXXTOOLS_UNIT_ASSERT_EQUALS(tz.toUtc(lt, false).toString(), "2018-10-28 01:30:06");
}

void TzTest::utc2local()
{
    {
        cxxtools::Tz tz("Europe/Berlin");
        cxxtools::UtcDateTime ut(2018, 11, 7, 13, 0, 5);
        cxxtools::LocalDateTime lt = tz.toLocal(ut);

        CXXTOOLS_UNIT_ASSERT_EQUALS(lt.toString(), cxxtools::LocalDateTime(2018, 11, 7, 14, 0, 5).toString());
    }

    {
        cxxtools::Tz tz("Pacific/Samoa");
        cxxtools::UtcDateTime ut(2018, 11, 7, 13, 10, 5);
        cxxtools::LocalDateTime lt = tz.toLocal(ut);

        CXXTOOLS_UNIT_ASSERT_EQUALS(lt.toString(), cxxtools::LocalDateTime(2018, 11, 7, 2, 10, 5).toString());
    }
}

void TzTest::offset()
{
    cxxtools::Tz tz("Europe/Berlin");
    cxxtools::Hours offset = tz.offset(cxxtools::UtcDateTime(2018, 11, 7, 13, 0, 6));
    CXXTOOLS_UNIT_ASSERT_EQUALS(offset, cxxtools::Hours(1));
}

void TzTest::isDst()
{
    cxxtools::Tz tz("Europe/Berlin");
    bool dst = tz.toLocal(cxxtools::UtcDateTime(2018, 11, 7, 13, 0, 4)).isdst();
    CXXTOOLS_UNIT_ASSERT(!dst);

    dst = tz.toLocal(cxxtools::UtcDateTime(2018, 8, 7, 13, 0, 4)).isdst();
    CXXTOOLS_UNIT_ASSERT(dst);
}

cxxtools::unit::RegisterTest<TzTest> register_TzTest;
