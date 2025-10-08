#include <cxxtools/unit/testsuite.h>
#include <cxxtools/unit/registertest.h>
#include <cxxtools/posix/commandoutput.h>

class CommandOutputTest : public cxxtools::unit::TestSuite
{
public:
    CommandOutputTest()
        : cxxtools::unit::TestSuite("commandoutput")
    {
        registerMethod("echo", *this, &CommandOutputTest::echo);
    }

    void echo();
};

void CommandOutputTest::echo()
{
    cxxtools::posix::CommandOutput cmd("/bin/echo");
    cmd.push_back("Hello");
    cmd.run();

    std::string value;
    cmd >> value;

    CXXTOOLS_UNIT_ASSERT_EQUALS(value, "Hello");
}

cxxtools::unit::RegisterTest<CommandOutputTest> register_CommandOutputTest;
