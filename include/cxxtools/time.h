/*
 * Copyright (C) 2006 by Tommi Maekitalo
 * Copyright (C) 2006-2008 by Marc Boris Duerner
 * Copyright (C) 2006 by Stefan Bueder
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
#ifndef CXXTOOLS_TIME_H
#define CXXTOOLS_TIME_H

#include <cxxtools/timespan.h>
#include <string>
#include <stdexcept>
#include <stdint.h>

namespace cxxtools
{

class SerializationInfo;

class InvalidTime : public std::invalid_argument
{
    public:
        InvalidTime();

        ~InvalidTime() throw()
        {}
};

/** @brief %Time expressed in hours, minutes, seconds and milliseconds
    @ingroup DateTime
*/
class Time
{
    public:
        static const unsigned MaxHours         = 23;
        static const unsigned HoursPerDay      = 24;
        static const unsigned MaxMinutes       = 59;
        static const unsigned MinutesPerHour   = 60;
        static const unsigned MinutesPerDay    = 1440;
        static const unsigned MaxSeconds       = 59;
        static const unsigned SecondsPerDay    = 86400;
        static const unsigned SecondsPerHour   = 3600;
        static const unsigned SecondsPerMinute = 60;
        static const unsigned MSecsPerDay      = 86400000;
        static const unsigned MSecsPerHour     = 3600000;
        static const unsigned MSecsPerMinute   = 60000;
        static const unsigned MSecsPerSecond   = 1000;
        static const unsigned long USecsPerDay = static_cast<unsigned long>(MSecsPerDay) * 1000;

        /** \brief Creates a Time set to zero.
        */
        Time()
        : _usecs(0)
        {}

        /** \brief create Time from string using format

            Valid format codes are:

              %H   hours (0-23)
              %I   hours (0-11)
              %M   minutes
              %S   seconds
              %j   milliseconds (1-3 digits, optionally leading '.')
              %J   milliseconds (1-3 digits, with leading '.')
              %K   milliseconds (3 digits, with leading '.')
              %k   milliseconds (3 digits)
              %p   AM/PM
              ?    arbitrary character
              *    skip non digit characters
         */
        explicit Time(const std::string& d, const std::string& fmt = "%H:%M:%S%j");

        /** \brief Creates a Time from given values.

            InvalidTime is thrown if one or more of the values are out of range
        */
        Time(unsigned h, unsigned m, unsigned s = 0, unsigned ms = 0, unsigned usec = 0)
        {
            set(h, m, s, ms, usec);
        }

        /** \brief Returns the hour-part of the Time.
        */
        unsigned hour() const
        {
            return unsigned(_usecs / 1000 / 1000 / 60 / 60);
        }

        /** \brief Returns the minute-part of the Time.
        */
        unsigned minute() const
        {
            return unsigned(_usecs / 1000 / 1000 / 60) % 60;
        }

        /** \brief Returns the second-part of the Time.
        */
        unsigned second() const
        {
            return unsigned(_usecs / 1000 / 1000) % 60;
        }

        /** \brief Returns the millisecond-part of the Time.
        */
        unsigned msec() const
        {
            return unsigned(_usecs / 1000) % 1000;
        }

        /** \brief Returns the microseconds of the Time.
         *
         *  Note that the microsoecnds contain the milliseconds also.
        */
        unsigned usec() const
        {
            return unsigned(_usecs % 1000000);
        }

        unsigned totalMSecs() const
        { return _usecs / 1000; }

        unsigned long totalUSecs() const
        { return _usecs; }

        void setTotalMSecs(unsigned msecs)
        { _usecs = msecs * 1000; }

        void setTotalUSecs(unsigned long m)
        { _usecs = m; }

        /** \brief Sets the time.

            Sets the time to a new hour, minute, second, milli-second.
            InvalidTime is thrown if one or more of the values are out of range
        */
        void set(unsigned hour, unsigned min, unsigned sec, unsigned msec = 0, unsigned usec = 0)
        {
            if ( ! isValid(hour, min, sec, msec, usec) )
            {
                throw InvalidTime();
            }

            _usecs = (((((static_cast<unsigned long>(hour) * 60 + min) * 60) + sec) * 1000) + msec) * 1000 + usec;
        }

        /** @brief Get the time values

            Gets the hour, minute, second and millisecond parts of the time.
        */
        void get(unsigned& h, unsigned& m, unsigned& s, unsigned& ms) const
        {
            h = hour();
            m = minute();
            s = second();
            ms = msec();
        }

        /** \brief format Time into a string using a format string

            Valid format codes are:

              %H   hours (0-23)
              %H   hours (0-11)
              %M   minutes
              %S   seconds
              %j   milliseconds (1-3 digits, optionally leading '.')
              %J   milliseconds (1-3 digits, with leading '.')
              %K   milliseconds (3 digits, with leading '.')
              %k   milliseconds (3 digits)
              %p   am/pm
              %P   AM/PM
         */
        std::string toString(const std::string& fmt = "%H:%M:%S%j") const;

        /** @brief Assignment operator
        */
        Time& operator=(const Time& other)
        { _usecs=other._usecs; return *this; }

        /** @brief Equal comparison operator
        */
        bool operator==(const Time& other) const
        { return _usecs == other._usecs; }

        /** @brief Inequal comparison operator
        */
        bool operator!=(const Time& other) const
        { return _usecs != other._usecs; }

        /** @brief Less-than comparison operator
        */
        bool operator<(const Time& other) const
        { return _usecs < other._usecs; }

        /** @brief Less-than-or-equal comparison operator
        */
        bool operator<=(const Time& other) const
        { return _usecs <= other._usecs; }

        /** @brief Greater-than comparison operator
        */
        bool operator>(const Time& other) const
        { return _usecs > other._usecs; }

        /** @brief Greater-than-or-equal comparison operator
        */
        bool operator>=(const Time& other) const
        { return _usecs >= other._usecs; }

        /** @brief Assignment by sum operator
        */
        Time& operator+=(const Timespan& ts);

        /** @brief Assignment by difference operator
        */
        Time& operator-=(const Timespan& ts);

        /** @brief Addition operator
        */
        friend Time operator+(const Time& time, const Timespan& ts);

        /** @brief Subtraction operator
        */
        friend Time operator-(const Time& time, const Timespan& ts);

        /** @brief Subtraction operator
        */
        friend Timespan operator-(const Time& a, const Time& b)
        {
            return Microseconds(
                a._usecs >= b._usecs
                    ? double(a._usecs - b._usecs)
                    : -double(b._usecs - a._usecs));
        }

        /** \brief Returns the time in ISO-format (hh:mm:ss.hhh)
        */
        std::string toIsoString() const
        { return toString(); }

        /** \brief Returns true if values are a valid time
        */
        static bool isValid(unsigned h, unsigned m, unsigned s, unsigned ms)
        {
            return h < 24 && m < 60 && s < 60 && ms < 1000;
        }

        /** \brief Returns true if values are a valid time
        */
        static bool isValid(unsigned h, unsigned m, unsigned s, unsigned ms, unsigned usec)
        {
            return h < 24 && m < 60 && s < 60 && ms < 1000 && usec < 1000000;
        }

        /** \brief Convert from an ISO time string

            Interprets the passed string as a time-string in ISO-format
            (hh:mm:ss.hhh) and returns a Time-object. If the string is not
            in ISO-format, InvalidTime is thrown.
        */
        static Time fromIsoString(const std::string& s)
        { return Time(s); }

    private:
        //! @internal
        unsigned long _usecs;
    };

    void operator >>=(const SerializationInfo& si, Time& time);

    void operator <<=(SerializationInfo& si, const Time& time);

    /** @brief Addition operator
    */
    inline Time operator+(const Time& time, const Timespan& ts)
    {
        Time t(time);
        t += ts;
        return t;
    }

    /** @brief Subtraction operator
    */
    inline Time operator-(const Time& time, const Timespan& ts)
    {
        Time t(time);
        t -= ts;
        return t;
    }

}

#endif // CXXTOOLS_TIME_H
