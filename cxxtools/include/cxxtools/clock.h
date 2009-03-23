/*
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

        /** @brief Returns the system time
        */
        static DateTime getSystemTime();

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
