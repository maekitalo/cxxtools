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

#include <cxxtools/api.h>
#include <cxxtools/timespan.h>
#include <string>
#include <stdexcept>

namespace cxxtools
{

class SerializationInfo;

class CXXTOOLS_API InvalidTime : public std::invalid_argument
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

        /** \brief Creates a Time set to zero.
        */
        Time()
        : _msecs(0)
        {}

        /** \brief Creates a Time from given values.

            InvalidTime is thrown if one or more of the values are out of range
        */
        inline Time(unsigned h, unsigned m, unsigned s = 0, unsigned ms = 0)
        {
            set(h, m, s, ms);
        }

        /** \brief Returns the hour-part of the Time.
        */
        unsigned hour() const
        {
            return _msecs / MSecsPerHour;
        }

        /** \brief Returns the minute-part of the Time.
        */
        unsigned minute() const
        {
            return (_msecs % MSecsPerHour) / MSecsPerMinute;
        }

        /** \brief Returns the second-part of the Time.
        */
        unsigned second() const
        {
            return (_msecs / 1000) % SecondsPerMinute;
        }

        /** \brief Returns the millisecond-part of the Time.
        */
        unsigned msec() const
        {
            return _msecs % 1000;
        }

        unsigned totalMSecs() const
        { return _msecs; }

        void setTotalMSecs(unsigned msecs)
        { _msecs = msecs; }

        /** \brief Sets the time.

            Sets the time to a new hour, minute, second, milli-second.
            InvalidTime is thrown if one or more of the values are out of range
        */
        void set(unsigned hour, unsigned min, unsigned sec, unsigned msec = 0)
        {
            if ( ! isValid(hour, min, sec , msec) )
            {
                throw InvalidTime();
            }

            _msecs = (hour*SecondsPerHour + min*SecondsPerMinute + sec) * 1000 + msec;
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

        /** @brief Adds seconds to the time

            This method does not change the time, but returns the time
            with the seconds added.
        */
        Time addSecs(int secs) const
        {
            return addMSecs(secs * 1000);
        }

        /** @brief Determines seconds until another time
        */
        int secsTo(const Time &t) const
        {
            return static_cast<int>( msecsTo(t) / 1000 );
        }

        /** @brief Adds milliseconds to the time

            This method does not change the time, but returns the time
            with the milliseconds added.
        */
        inline Time addMSecs(cxxtools::int64_t ms) const
        {
            Time t;
            if (ms < 0)
            {
                cxxtools::int64_t negdays = (MSecsPerDay - ms) / MSecsPerDay;
                t._msecs = static_cast<unsigned>((_msecs + ms + negdays * MSecsPerDay) % MSecsPerDay);
            }
            else
            {
                t._msecs = static_cast<unsigned>((_msecs + ms) % MSecsPerDay);
            }

            return t;
        }

        /** @brief Determines milliseconds until another time
        */
        cxxtools::int64_t msecsTo(const Time &t) const
        {
            if(t._msecs > _msecs)
                return t._msecs - _msecs;

            return MSecsPerDay - (_msecs - t._msecs);
        }

        /** @brief Assignment operator
        */
        Time& operator=(const Time& other)
        { _msecs=other._msecs; return *this; }

        /** @brief Equal comparison operator
        */
        bool operator==(const Time& other) const
        { return _msecs == other._msecs; }

        /** @brief Inequal comparison operator
        */
        bool operator!=(const Time& other) const
        { return _msecs != other._msecs; }

        /** @brief Less-than comparison operator
        */
        bool operator<(const Time& other) const
        { return _msecs < other._msecs; }

        /** @brief Less-than-or-equal comparison operator
        */
        bool operator<=(const Time& other) const
        { return _msecs <= other._msecs; }

        /** @brief Greater-than comparison operator
        */
        bool operator>(const Time& other) const
        { return _msecs > other._msecs; }

        /** @brief Greater-than-or-equal comparison operator
        */
        bool operator>=(const Time& other) const
        { return _msecs >= other._msecs; }

        /** @brief Assignment by sum operator
        */
        Time& operator+=(const Timespan& ts)
        {
            cxxtools::int64_t msecs = ( _msecs + ts.totalMSecs() ) % MSecsPerDay;
            msecs = msecs < 0 ? MSecsPerDay + msecs : msecs;
            _msecs = static_cast<unsigned>(msecs);
            return *this;
        }

        /** @brief Assignment by difference operator
        */
        Time& operator-=(const Timespan& ts)
        {
            cxxtools::int64_t msecs = ( _msecs - ts.totalMSecs() ) % MSecsPerDay;
            msecs = msecs < 0 ? MSecsPerDay + msecs : msecs;
            _msecs = static_cast<unsigned>(msecs);
            return *this;
        }

        /** @brief Addition operator
        */
        friend Time operator+(const Time& time, const Timespan& ts)
        {
            return time.addMSecs( ts.totalMSecs() );
        }

        /** @brief Substraction operator
        */
        friend Time operator-(const Time& time, const Timespan& ts)
        {
            return time.addMSecs( -ts.totalMSecs() );
        }

        /** @brief Substraction operator
        */
        friend Timespan operator-(const Time& a, const Time& b)
        {
            return b.msecsTo(a) * 1000;
        }

        /** \brief Returns the time in ISO-format (hh:mm:ss.hhh)
        */
        std::string toIsoString() const;

        /** \brief Returns true if values are a valid time
        */
        static bool isValid(unsigned h, unsigned m, unsigned s, unsigned ms)
        {
            return h < 24 && m < 60 && s < 60 && ms < 1000;
        }

        /** \brief Convert from an ISO time string

            Interprets the passed string as a time-string in ISO-format
            (hh:mm:ss.hhh) and returns a Time-object. If the string is not
            in ISO-format, InvalidTime is thrown.
        */
        static Time fromIsoString(const std::string& s);

    private:
        //! @internal
        unsigned _msecs;
    };

    CXXTOOLS_API void operator >>=(const SerializationInfo& si, Time& time);

    CXXTOOLS_API void operator <<=(SerializationInfo& si, const Time& time);

    CXXTOOLS_API void convert(std::string& str, const cxxtools::Time& time);

    CXXTOOLS_API void convert(cxxtools::Time& time, const std::string& s);

    inline std::string Time::toIsoString() const
    {
        std::string str;
        convert(str, *this);
        return str;
    }

    inline Time Time::fromIsoString(const std::string& s)
    {
        Time time;
        convert(time, s);
        return time;
    }
}

#endif // CXXTOOLS_TIME_H
