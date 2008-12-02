#ifndef CXXTOOLS_CLOCK_H
#define CXXTOOLS_CLOCK_H

#include <cxxtools/api.h>
#include <cxxtools/types.h>
#include <cxxtools/datetime.h>
#include <cxxtools/timespan.h>

namespace cxxtools {

/** @brief Measures time intervals

    The clock class can be used like a stop-watch by calling Clock::start()
    and Clock::stop(). The latter method returns the elapsed time.
*/
class CXXTOOLS_API Clock
{
    public:
        /** @brief Contructs a Clock
        */
        Clock();

        /** @brief Destructor
        */
        ~Clock();

        /** @brief Start the clock
        */
        void start();

        /** @brief Stop the clock

            Returns the elapsed time since start was called.
        */
        Timespan stop();

        /** @brief Returns the current local time
        */
        static DateTime getLocalTime();

        /** @brief Returns the timespan since a fixed point in the past

            The getSystemTicks function retrieves the system ticks, in milliseconds.
            The system time is the time elapsed since i.e. the system was started, or
            the unix epoch or some other fixed point in the past.
        */
        static Timespan getSystemTicks();

    private:
        class ClockImpl *_impl;
};

} //namespace cxxtools

#endif // CXXTOOLS_CLOCK_H
