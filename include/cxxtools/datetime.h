/*
 * Copyright (C) 2006 by Tommi Maekitalo
 * Copyright (C) 2006 by Marc Boris Duerner
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
#ifndef CXXTOOLS_DATETIME_H
#define CXXTOOLS_DATETIME_H

#include <cxxtools/time.h>
#include <cxxtools/date.h>
#include <string>

namespace cxxtools
{

class SerializationInfo;
class LocalDateTime;
class UtcDateTime;

/** @brief Combined %Date and %Time value
    @ingroup DateTime
*/
class DateTime
{
    public:
        DateTime()
        { }

        /** \brief create DateTime from string using format

            Valid format codes are:

              %Y   4 digit year
              %y   2 digit year
              %m   month (1-12)
              %2m  month (01-12)
              %O   english monthname (Jan-Dec)
              %d   day (1-31)
              %2d  day (01-31)
              %H   hours (0-23)
              %2H  hours (00-23)
              %I   hours (0-11)
              %2I  hours (00-11)
              %M   minutes (0-59)
              %2M  minutes (00-59)
              %S   seconds (0-59)
              %2S  seconds (00-59)
              %j   fractional seconds (1-6 digits, optionally leading '.')
              %J   fractional seconds (1-6 digits, with leading '.')
              %K   milliseconds (3 digits, with leading '.')
              %k   milliseconds (3 digits)
              %U   microseconds (6 digits, with leading '.')
              %u   microseconds (6 digits)
              %p   AM/PM
              ?    arbitrary character
              *    skip non digit characters
              #    skip word
         */
        explicit DateTime(const std::string& d, const std::string& fmt = "%Y-%m-%d %H:%M:%S%j");

        DateTime(int year, unsigned month, unsigned day,
                 unsigned hour, unsigned minute, 
                 unsigned second, unsigned msec = 0, unsigned usec = 0)
        : _date(year, month, day)
        , _time(hour, minute, second, msec, usec)
        { }

        DateTime(const Date& date, const Time& time)
        : _date(date),
          _time(time)
        { }

        /** @brief Creates a DateTime object relative to the Unix epoch.

            The DateTime will be relative to the unix epoch (Jan 1st 1970)
            by the milli seconds specified by \a msecsSinceEpoch. The
            construction does not take care of any time zones. I.e. the
            milliseconds will be treated as if they were in the same time
            zone as the reference (January 1st 1970). Thus specifying a
            "time zoned" millisecond value will lead to a "time zoned"
            DateTime. And accordingly a "GMT" millisecond value will lead
            to a "GMT" DateTime.
        */
        static UtcDateTime fromMSecsSinceEpoch(cxxtools::Milliseconds sinceEpoch);

        /// Returns the GMT time
        static UtcDateTime gmtime();

        /// Returns the local time
        static LocalDateTime localtime();

        /// Sets the date and time.
        void set(int year, unsigned month, unsigned day,
                 unsigned hour, unsigned min, unsigned sec, unsigned msec = 0, unsigned usec = 0);

        /// Returns the components of the date time.
        void get(int& year, unsigned& month, unsigned& day,
                 unsigned& hour, unsigned& min, unsigned& sec, unsigned& msec) const;

        /// Returns the components of the date time.
        void get(int& year, unsigned& month, unsigned& day,
                 unsigned& hour, unsigned& min, unsigned& sec, unsigned& msec, unsigned& usec) const;

        /// Returns the date part of the DateTime object.
        const Date& date() const
        { return _date; }

        /// Set the date part and keeps the time of the DateTime object.
        DateTime& setDate(const Date& date)
        { _date = date; return *this; }

        /// Returns the time part of the DateTime object.
        const Time& time() const
        { return _time; }

        /// Set the time part and keeps the date of the DateTime object.
        DateTime& setTime(const Time& time)
        { _time = time; return *this; }

        /** @brief Returns the day part of the date.
        */
        unsigned day() const
        { return date().day(); }

        /** @brief Returns the month part of the date.
        */
        unsigned month() const
        { return date().month(); }

        /** @brief Returns the year part of the date.
        */
        int year() const
        { return date().year(); }

        /** @brief Return day of the week, starting with sunday (=0)
        */
        unsigned dayOfWeek() const
        { return date().dayOfWeek(); }

        /** \brief Returns the hour part of the Time.
        */
        unsigned hour() const
        { return time().hour(); }

        /** \brief Returns the minute part of the Time.
        */
        unsigned minute() const
        { return time().minute(); }

        /** \brief Returns the second part of the Time.
        */
        unsigned second() const
        { return time().second(); }

        /** \brief Returns the millisecond part of the Time.
        */
        unsigned msec() const
        { return time().msec(); }

        /** @brief Returns the milliseconds relative to the Unix epoch.

            The calculation does currently not take care of any time zones.
            I.e. the milliseconds will be calculated as if they were in the
            same time zone as the reference (January 1st 1970). Thus calling
            this API on a "time zoned" DateTime will lead to a "time zoned"
            millisecond value. And  accordingly calling this API on a "GMT"
            DateTime will lead to a "GMT" millisecond value.
        */
        Milliseconds msecsSinceEpoch() const;

        /** \brief format Date into a string using a format string

            Valid format codes are:

              %d   day (01-31)
              %1d  day (1-31)
              %m   month (01-12)
              %1m  month (1-12)
              %O   english monthname (Jan-Dec)
              %Y   4 digit year
              %y   2 digit year
              %w   day of week (0-6 sunday=6)
              %W   day of week (1-7 sunday=7)
              %N   day of week as english abbrivation (Mon-Sun)
              %H   hours (00-23)
              %1H  hours (0-23)
              %I   hours (00-11)
              %1I  hours (0-11)
              %M   minutes (00-59)
              %1M  minutes (0-59)
              %S   seconds (00-59)
              %1S  seconds (0-59)
              %j   fractional seconds (1-6 digits, optionally leading '.')
              %J   fractional seconds (1-6 digits, with leading '.')
              %K   milliseconds (3 digits, with leading '.')
              %k   milliseconds (3 digits)
              %U   microseconds (6 digits, with leading '.')
              %u   microseconds (6 digits)
              %p   am/pm
              %P   AM/PM
         */
        std::string toString(const std::string& fmt = "%Y-%m-%d %H:%M:%S%j") const;

        std::string toIsoString() const
        { return toString(); }

        static DateTime fromIsoString(const std::string& s)
        { return DateTime(s); }

        static bool isValid(int year, unsigned month, unsigned day,
                            unsigned hour, unsigned minute, unsigned second, unsigned msec = 0);


        bool operator==(const DateTime& rhs) const
        {
            return _date == rhs._date && _time == rhs._time ;
        }

        bool operator!=(const DateTime& rhs) const
        {
            return !operator==(rhs);
        }

        /** @brief Assignment by sum operator
        */
        DateTime& operator+=(const Timespan& ts);

        /** @brief Assignment by difference operator
        */
        DateTime& operator-=(const Timespan& ts)
        { return operator+= (-ts); }

        friend Timespan operator-(const DateTime& first, const DateTime& second);

        friend DateTime operator+(const DateTime& dt, const Timespan& ts);

        friend DateTime operator-(const DateTime& dt, const Timespan& ts);

        bool operator< (const DateTime& dt) const
        {
            return _date < dt._date
                || (_date == dt._date
                  && _time < dt._time);
        }

        bool operator<= (const DateTime& dt) const
        {
            return _date < dt._date
                || (_date == dt._date
                  && _time <= dt._time);
        }

        bool operator> (const DateTime& dt) const
        {
            return _date > dt._date
                || (_date == dt._date
                  && _time > dt._time);
        }

        bool operator>= (const DateTime& dt) const
        {
            return _date > dt._date
                || (_date == dt._date
                  && _time >= dt._time);
        }

    private:
        Date _date;
        Time _time;
};

void operator >>=(const SerializationInfo& si, DateTime& dt);

void operator <<=(SerializationInfo& si, const DateTime& dt);

/// Same ad DateTime - just a separate type for local date time better type safety
class LocalDateTime : public DateTime
{
public:
    LocalDateTime() { }

    explicit LocalDateTime(const std::string& d, const std::string& fmt = "%Y-%m-%d %H:%M:%S%j")
        : DateTime(d, fmt)
        { }

    LocalDateTime(int year, unsigned month, unsigned day,
             unsigned hour, unsigned minute,
             unsigned second, unsigned msec = 0, unsigned usec = 0)
        : DateTime(year, month, day, hour, minute, second, msec, usec)
        { }

    LocalDateTime(const Date& date, const Time& time)
        : DateTime(date, time)
        { }

    explicit LocalDateTime(const DateTime& dt)
        : DateTime(dt)
        { }
};

/// Same ad DateTime - just a separate type for utc date time better type safety
class UtcDateTime : public DateTime
{
public:
    UtcDateTime() { }

    explicit UtcDateTime(const std::string& d, const std::string& fmt = "%Y-%m-%d %H:%M:%S%j")
        : DateTime(d, fmt)
        { }

    UtcDateTime(int year, unsigned month, unsigned day,
             unsigned hour, unsigned minute,
             unsigned second, unsigned msec = 0, unsigned usec = 0)
        : DateTime(year, month, day, hour, minute, second, msec, usec)
        { }

    UtcDateTime(const Date& date, const Time& time)
        : DateTime(date, time)
        { }

    explicit UtcDateTime(const DateTime& dt)
        : DateTime(dt)
        { }
};

inline void operator>>= (const SerializationInfo& si, LocalDateTime& dt)
{ si >>= static_cast<DateTime&>(dt); }

inline void operator<<= (SerializationInfo& si, const LocalDateTime& dt)
{ si <<= static_cast<const DateTime&>(dt); }

inline void operator>>= (const SerializationInfo& si, UtcDateTime& dt)
{ si >>= static_cast<DateTime&>(dt); }

inline void operator<<= (SerializationInfo& si, const UtcDateTime& dt)
{ si <<= static_cast<const DateTime&>(dt); }


inline void DateTime::set(int year, unsigned month, unsigned day,
                   unsigned hour, unsigned minute, unsigned second, unsigned msec, unsigned usec)
{
    _date.set(year, month, day);
    _time.set(hour, minute, second, msec, usec);
}


inline void DateTime::get(int& y, unsigned& month, unsigned& d,
                   unsigned& h, unsigned& min, unsigned& s, unsigned& ms) const
{
    _date.get(y, month, d);
    _time.get(h, min, s, ms);
}


inline void DateTime::get(int& y, unsigned& month, unsigned& d,
                   unsigned& h, unsigned& min, unsigned& s, unsigned& ms, unsigned& usec) const
{
    _date.get(y, month, d);
    _time.get(h, min, s, ms, usec);
}


inline bool DateTime::isValid(int year, unsigned month, unsigned day,
                       unsigned hour, unsigned minute, unsigned second, unsigned msec)
{
    return Date::isValid(year, month, day) && Time::isValid(hour, minute, second, msec);
}

}

#endif // CXXTOOLS_DATETIME_H
