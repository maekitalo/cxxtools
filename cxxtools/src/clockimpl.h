#include "cxxtools/datetime.h"
#include "cxxtools/timespan.h"
#include <sys/time.h>
#include <time.h>

namespace cxxtools {

class ClockImpl
{
    public:
        ClockImpl();

        ~ClockImpl();

        void start ();

        Timespan stop();

        static DateTime getLocalTime();

        static Timespan getSystemTicks();

    private:
        struct timeval  _startTime;
        struct timeval  _stopTime;
        struct timezone _timeZone;
};

} // namespace cxxtools

