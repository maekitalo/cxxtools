/***************************************************************************
 *   Copyright (C) 2004-2008 Marc Boris Duerner                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef CXXTOOLS_DATE_H
#define CXXTOOLS_DATE_H

#include <cxxtools/api.h>
#include <cxxtools/sourceinfo.h>
#include <string>
#include <stdexcept>

namespace cxxtools {

class SerializationInfo;


class CXXTOOLS_API InvalidDate : public std::invalid_argument
{
    public:
        InvalidDate(const SourceInfo& si);

        ~InvalidDate() throw();
};

/*
  Notes:
  - Henry F. Fliegel and Thomas C. Van Flandern, "A Machine Algorithm for
    Processing Calendar Dates". CACM, Vol. 11, No. 10, October 1968, pp 657.
*/
/** @brief %Date expressed in year, month, and day
    @ingroup DateTime
*/
class CXXTOOLS_API Date
{
    public:
        enum Month
        {
            Jan = 1, Feb, Mar,  Apr, May, Jun,
            Jul, Aug, Sep, Oct, Nov, Dec
        };

        enum WeekDay
        {
            Sun = 0, Mon, Tue, Wed, Thu, Fri, Sat
        };

        /**
        * @brief The number of days of an ordinary year.
        */
        static const unsigned DaysPerYear = 365;

        /**
        * @brief The number of days of a leap year.
        */
        static const unsigned DaysPerLeapYear = 366;

        /**
        * @brief The number of days of a January.
        */
        static const unsigned DaysOfJan = 31;

        /**
        * @brief The number of days of a February.
        */
        static const unsigned DaysOfFeb = 28;

        /**
        * @brief The number of days of a February in a leap year.
        */
        static const unsigned DaysOfLeapFeb = 29;

        /**
        * @brief The number of days of a March.
        */
        static const unsigned DaysOfMar = 31;

        /**
        * @brief The number of days of a April.
        */
        static const unsigned DaysOfApr = 30;

        /**
        * @brief The number of days of a May.
        */
        static const unsigned DaysOfMay = 31;

        /**
        * @brief The number of days of a June.
        */
        static const unsigned DaysOfJun = 30;

        /**
        * @brief The number of days of a July.
        */
        static const unsigned DaysOfJul = 31;

        /**
        * @brief The number of days of a August.
        */
        static const unsigned DaysOfAug = 31;

        /**
        * @brief The number of days of a September.
        */
        static const unsigned DaysOfSep = 30;

        /**
        * @brief The number of days of a October.
        */
        static const unsigned DaysOfOct = 31;

        /**
        * @brief The number of days of a November.
        */
        static const unsigned DaysOfNov = 30;

        /**
        * @brief The number of days of a December.
        */
        static const unsigned DaysOfDec = 31;

    public:
        /** \brief Default constructor.

            The default constructed date is undefined.
        */
        Date()
        : _julian(0)
        {}

        /** \brief Copy constructor.
        */
        Date(const Date& date)
        : _julian(date._julian)
        {}

        /** \brief Constructs a Date from given values

            Sets the date to a new year, month and day.
            InvalidDate is thrown if any of the values is out of range
        */
        Date(int y, unsigned m, unsigned d);

        /** \brief Constructs a Date from a julian day
        */
        explicit Date(unsigned julianDays)
        : _julian(julianDays)
        {}

        /** @brief Sets the Date to a julian day
        */
        void setJulian(unsigned d)
        { _julian=d; }

        /** @brief Returns the Date as a julian day
        */
        unsigned julian() const
        { return _julian; }

        /** \brief Sets the date to a year, month and day

            Sets the date to a new year, month and day.
            InvalidDate is thrown if any of the values is out of range
        */
        void set(int year, unsigned month, unsigned day);

        /** @brief Gets the year, month and day
        */
        void get(int& year, unsigned& month, unsigned& day) const;

        /** \brief Returns the day-part of the date.
        */
        unsigned day() const;

        /** \brief Returns the month-part of the date.
        */
        unsigned month() const;

        /** \brief Returns the year-part of the date.
        */
        int year() const;

        /** @brief Return day of the week, starting with sunday
        */
        unsigned dayOfWeek() const;

        /** @brief Returns the days of the month of the date
        */
        unsigned daysInMonth() const;

        /** @brief Returns the day of the year
        */
        unsigned dayOfYear() const;

        /** @brief Returns true if the date is in a leap year
        */
        bool leapYear() const;

        /** @brief Assignment operator
        */
        Date& operator=(const Date& date)
        { _julian = date._julian; return *this; }

        /** @brief Add days to the date
        */
        Date& operator+=(int days)
        { _julian += days; return *this; }

        /** @brief Substract days from the date
        */
        Date& operator-=(int days)
        { _julian -= days; return *this; }

        /** @brief Increments the date by one day
        */
        Date& operator++()
        { _julian++; return *this; }

        /** @brief Decrements the date by one day
        */
        Date& operator--()
        { _julian--; return *this; }

        /** @brief Returns true if the dates are equal
        */
        bool operator==(const Date& date) const
        { return _julian==date._julian; }

        /** @brief Returns true if the dates are not equal
        */
        bool operator!=(const Date& date) const
        { return _julian!=date._julian; }

        /** @brief Less-than comparison operator
        */
        bool operator<(const Date& date) const
        { return _julian<date._julian; }

        /** @brief Less-than-equal comparison operator
        */
        bool operator<=(const Date& date) const
        { return _julian<=date._julian; }

        /** @brief Greater-than comparison operator
        */
        bool operator>(const Date& date) const
        { return _julian>date._julian; }

        /** @brief Greater-than-equal comparison operator
        */
        bool operator>=(const Date& date) const
        { return _julian>=date._julian; }

        friend inline Date operator+(const Date& d, int days);

        friend inline Date operator+(int days, const Date& d);

        friend inline int operator-(const Date& a, const Date& b);

        /** \brief Returns the date in ISO-format

            Converts the date in ISO-format (yyyy-mm-dd).

            \return Date as iso formated string.
        */
        std::string toIsoString() const;

        /** \brief Interprets a string as a date-string in ISO-format

            Interprets a string as a date-string in ISO-format (yyyy-mm-dd) and
            returns a Date-object. When the string is not in ISO-format, an
            exception is thrown.

            \param s Iso formated date string.
            \return Date result
            \throw IllegalArgument
        */
        static Date fromIsoString(const std::string& s);

    public:
        /** \brief Returns true if values describe a valid date
        */
        static bool isValid(int y, int m, int d);

        /** @brief Returns true if the year is in a leap year
        */
        static bool leapYear(int year);

        /** @brief Converts a gergorian date to a julian day
        */
        static void greg2jul(unsigned& jd, int y, int m, int d);

        /** @brief Converts a julian day to a gregorian date
        */
        static void jul2greg(unsigned jd, int& y, int& m, int& d);

    private:
        //! @internal
        unsigned _julian;
};

inline Date operator+(const Date& d, int days)
{ return Date(d._julian + days); }

inline Date operator+(int days, const Date& d)
{ return Date(days + d._julian); }

inline int operator-(const Date& a, const Date& b)
{ return a._julian - b._julian; }

}

#endif
