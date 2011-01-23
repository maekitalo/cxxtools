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

#include <cxxtools/api.h>
#include <cxxtools/time.h>
#include <cxxtools/date.h>
#include <string>
#include <map>

namespace cxxtools
{

class SerializationInfo;

/** @brief Combined %Date and %Time value
    @ingroup DateTime
*/
class DateTime
{
    public:
		DateTime()
		{ }

		DateTime(int year, unsigned month, unsigned day,
		                   unsigned hour = 0, unsigned minute = 0, 
		                   unsigned second = 0, unsigned msec = 0)
		: _date(year, month, day)
		, _time(hour, minute, second, msec)
		{ }

		DateTime(const DateTime& dateTime)
		: _date( dateTime.date() )
		, _time( dateTime.time() )
		{ }

        DateTime& operator=(const DateTime& dateTime);

        ~DateTime()
		{}

        static DateTime fromJulianDays(unsigned julianDays)
        {
            return DateTime(julianDays);
        }

        /** @brief Creates a DateTime object relative to the Unix epoch.

            The DateTime will be relative to the unix-epoch (Jan 1st 1970)
            by the milli-seconds specified by \a msecsSinceEpoch. The
            construction does not take care of any time zones. I.e. the
            milliseconds will be treated as if they were in the same time
            zone as the reference (January 1st 1970). Thus specifying a
            "time-zoned" millisecond value will lead to a "time-zoned"
            DateTime. And accordingly a "GMT" millisecond value will lead
            to a "GMT" DateTime.
        */
        static inline DateTime fromMSecsSinceEpoch(const cxxtools::int64_t msecsSinceEpoch)
        {
            static const DateTime dt(1970, 1, 1);
            Timespan ts(msecsSinceEpoch*1000);
            return dt + ts;
        }

        //DateTime& operator=(const DateTime& dateTime);

        DateTime& operator=(unsigned julianDay);

        void set(int year, unsigned month, unsigned day,
                 unsigned hour = 0, unsigned min = 0, unsigned sec = 0, unsigned msec = 0);

        void get(int& year, unsigned& month, unsigned& day,
                 unsigned& hour, unsigned& min, unsigned& sec, unsigned& msec) const;

        const Date& date() const
        { return _date; }

        const Date& date()
        { return _date; }

        DateTime& setDate(const Date& date)
        { _date = date; return *this; }

        const Time& time() const
        { return _time; }

        const Time& time()
        { return _time; }

        DateTime& setTime(const Time& time)
        { _time = time; return *this; }

        /** @brief Returns the day-part of the date.
        */
        unsigned day() const
        { return date().day(); }

        /** @brief Returns the month-part of the date.
        */
        unsigned month() const
        { return date().month(); }

        /** @brief Returns the year-part of the date.
        */
        int year() const
        { return date().year(); }

        /** \brief Returns the hour-part of the Time.
        */
        unsigned hour() const
        { return time().hour(); }

        /** \brief Returns the minute-part of the Time.
        */
        unsigned minute() const
        { return time().minute(); }

        /** \brief Returns the second-part of the Time.
        */
        unsigned second() const
        { return time().second(); }

        /** \brief Returns the millisecond-part of the Time.
        */
        unsigned msec() const
        { return time().msec(); }

        /** @brief Returns the milliseconds relative to the Unix-epoch.

            The calculation does currently not take care of any time zones.
            I.e. the milliseconds will be calculated as if they were in the
            same time zone as the reference (January 1st 1970). Thus calling
            this API on a "time-zoned" DateTime will lead to a "time-zoned"
            millisecond value. And  accordingly calling this API on a "GMT"
            DateTime will lead to a "GMT" millisecond value.
        */
        cxxtools::int64_t msecsSinceEpoch() const
        {
            static const DateTime dt(1970, 1, 1);
            return (*this - dt).totalMSecs();
        }

        std::string toIsoString() const;

        static DateTime fromIsoString(const std::string& s);

        static bool isValid(int year, unsigned month, unsigned day,
                            unsigned hour, unsigned minute, unsigned second, unsigned msec);


        bool operator==(const DateTime& rhs) const
        {
            return !operator!=(rhs);
        }

        bool operator!=(const DateTime& rhs) const
        {
            return _date != rhs._date || _time != rhs._time ;
        }

        /** @brief Assignment by sum operator
        */
        DateTime& operator+=(const Timespan& ts)
        {
            cxxtools::int64_t totalMSecs = ts.totalMSecs();
            cxxtools::int64_t days = totalMSecs / Time::MSecsPerDay;
            cxxtools::int64_t overrun = totalMSecs % Time::MSecsPerDay;

            if( (-overrun) > _time.totalMSecs()  )
            {
                days -= 1;
            }
            else if( overrun + _time.totalMSecs() > Time::MSecsPerDay)
            {
                days += 1;
            }

            _date += static_cast<int>(days);
            _time += Timespan(overrun * 1000);
            return *this;
        }

        /** @brief Assignment by difference operator
        */
        DateTime& operator-=(const Timespan& ts)
        {
            cxxtools::int64_t totalMSecs = ts.totalMSecs();
            cxxtools::int64_t days = totalMSecs / Time::MSecsPerDay;
            cxxtools::int64_t overrun = totalMSecs % Time::MSecsPerDay;

            if( overrun > _time.totalMSecs() )
            {
                days += 1;
            }
            else if(_time.totalMSecs() - overrun > Time::MSecsPerDay)
            {
                days -= 1;
            }

            _date -= static_cast<int>(days);
            _time -= Timespan( overrun * 1000 );
            return *this;
        }

        friend Timespan operator-(const DateTime& first, const DateTime& second)
        {
            cxxtools::int64_t dayDiff      = cxxtools::int64_t( first.date().julian() ) -
                                       cxxtools::int64_t( second.date().julian() );

            cxxtools::int64_t milliSecDiff = cxxtools::int64_t( first.time().totalMSecs() ) -
                                       cxxtools::int64_t( second.time().totalMSecs() );

            cxxtools::int64_t result = (dayDiff * Time::MSecsPerDay + milliSecDiff) * 1000;

            return result;
        }

        friend DateTime operator+(const DateTime& dt, const Timespan& ts)
        {
            DateTime tmp = dt;
            tmp += ts;
            return tmp;
        }

        friend DateTime operator-(const DateTime& dt, const Timespan& ts)
        {
            DateTime tmp = dt;
            tmp -= ts;
            return tmp;
        }

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
        DateTime(unsigned jd)
        : _date(jd)
        {}

    private:
        Date _date;
        Time _time;
};

CXXTOOLS_API void operator >>=(const SerializationInfo& si, DateTime& dt);

CXXTOOLS_API void operator <<=(SerializationInfo& si, const DateTime& dt);

CXXTOOLS_API void convert(DateTime& dt, const std::string& s);

CXXTOOLS_API void convert(std::string& str, const DateTime& dt);


inline DateTime DateTime::fromIsoString(const std::string& s)
{
    DateTime dt;
    convert(dt, s);
    return dt;
}


inline std::string DateTime::toIsoString() const
{
    std::string str;
    convert(str, *this);
    return str;
}


inline DateTime& DateTime::operator=(const DateTime& dateTime)
{
	_date = dateTime.date();
	_time = dateTime.time();
	return *this;
}


inline DateTime& DateTime::operator=(unsigned julianDay)
{
    _time = Time(0, 0, 0, 0);
    _date.setJulian(julianDay);
    return *this;
}


inline void DateTime::set(int year, unsigned month, unsigned day,
                   unsigned hour, unsigned minute, unsigned second, unsigned msec)
{
    _date.set(year, month, day);
    _time.set(hour, minute, second, msec);
}


inline void DateTime::get(int& y, unsigned& month, unsigned& d,
                   unsigned& h, unsigned& min, unsigned& s, unsigned& ms) const
{
    _date.get(y, month, d);
    _time.get(h, min, s, ms);
}


inline bool DateTime::isValid(int year, unsigned month, unsigned day,
                       unsigned hour, unsigned minute, unsigned second, unsigned msec)
{
    return Date::isValid(year, month, day) && Time::isValid(hour, minute, second, msec);
}

}

#endif // CXXTOOLS_DATETIME_H
