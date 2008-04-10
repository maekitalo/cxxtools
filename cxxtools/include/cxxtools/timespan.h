/***************************************************************************
 *   Copyright (C) 2006-2008 by Marc Boris Duerner                         *
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
#ifndef cxxtools_Timespan_h
#define cxxtools_Timespan_h

#include <cxxtools/api.h>
#include <cxxtools/types.h>


namespace cxxtools {

/** @brief  Represents time spans up to microsecond resolution.
    @ingroup DateTime
*/
class CXXTOOLS_API Timespan
{
    public:
        //! @brief Creates a zero Timespan.
        Timespan();

        //! @brief Creates a Timespan.
        Timespan(cxxtools::int64_t microseconds);

        /** @brief Creates a Timespan.
            Useful for creating a Timespan from a struct timeval.
        */
        Timespan(long seconds, long microseconds);

        //! @brief Creates a Timespan.
        Timespan(int days, int hours, int minutes, int seconds, int microseconds);

        //! @brief Creates a Timespan from another one.
        Timespan(const Timespan& timespan);

        //! @brief Destroys the Timespan.
        ~Timespan();

        //! @brief Assignment operator.
        Timespan& operator=(const Timespan& timespan);

        //! @brief Assignment operator.
        Timespan& operator=(cxxtools::int64_t microseconds);

        //! @brief Assigns a new span.
        Timespan& set(int days, int hours, int minutes, int seconds, int microseconds);

        /** @brief Assigns a new span.
            Useful for assigning from a struct timeval.
        */
        Timespan& set(long seconds, long microseconds);

        //! @brief Swaps the Timespan with another one.
        void swap(Timespan& timespan);

        bool operator==(const Timespan& ts) const;

        bool operator!=(const Timespan& ts) const;

        bool operator>(const Timespan& ts) const;

        bool operator>=(const Timespan& ts) const;

        bool operator<(const Timespan& ts) const;

        bool operator<=(const Timespan& ts) const;

        bool operator==(cxxtools::int64_t microseconds) const;

        bool operator!=(cxxtools::int64_t microseconds) const;

        bool operator>(cxxtools::int64_t microseconds) const;

        bool operator>=(cxxtools::int64_t microseconds) const;

        bool operator<(cxxtools::int64_t microseconds) const;

        bool operator<=(cxxtools::int64_t microseconds) const;

        Timespan operator+(const Timespan& d) const;

        Timespan operator-(const Timespan& d) const;

        Timespan& operator+=(const Timespan& d);

        Timespan& operator-=(const Timespan& d);

        Timespan operator+(cxxtools::int64_t microseconds) const;

        Timespan operator-(cxxtools::int64_t microseconds) const;

        Timespan& operator+=(cxxtools::int64_t microseconds);

        Timespan& operator-=(cxxtools::int64_t microseconds);

        //! @brief Returns the number of days.
        int days() const;

        //! @brief Returns the number of hours (0 to 23).
        int hours() const;

        //! @brief Returns the total number of hours.
        int totalHours() const;

        //! @brief Returns the number of minutes (0 to 59).
        int minutes() const;

        //! @brief Returns the total number of minutes.
        int totalMinutes() const;

        //! @brief Returns the number of seconds (0 to 59).
        int seconds() const;

        //! @brief Returns the total number of seconds.
        int totalSeconds() const;

        //! @brief Returns the number of milliseconds (0 to 999).
        int msecs() const;

        //! @brief Returns the total number of milliseconds.
        cxxtools::int64_t totalMSecs() const;

        /** @brief Returns the fractions of a millisecond in microseconds (0 to 999).
        */ 
        int microseconds() const;

        /** @brief Returns the fractions of a second in microseconds (0 to 999).
        */ 
        int useconds() const;
 
        //! @brief Returns the total number of microseconds.
        cxxtools::int64_t totalMicroseconds() const;

        //! @brief The number of microseconds in a millisecond.
        static const cxxtools::int64_t Milliseconds;

        //! @brief The number of microseconds in a second.
        static const cxxtools::int64_t Seconds;

        //! @brief The number of microseconds in a minute.
        static const cxxtools::int64_t Minutes;

        //! @brief The number of microseconds in a hour.
        static const cxxtools::int64_t Hours;

        //! @brief The number of microseconds in a day.
        static const cxxtools::int64_t Days;

    private:
        cxxtools::int64_t _span;
};


inline int Timespan::days() const
{
	return int(_span/Days);
}


inline int Timespan::hours() const
{
	return int((_span/Hours) % 24);
}

	
inline int Timespan::totalHours() const
{
	return int(_span/Hours);
}

	
inline int Timespan::minutes() const
{
	return int((_span/Minutes) % 60);
}

	
inline int Timespan::totalMinutes() const
{
	return int(_span/Minutes);
}

	
inline int Timespan::seconds() const
{
	return int((_span/Seconds) % 60);
}

	
inline int Timespan::totalSeconds() const
{
	return int(_span/Seconds);
}

	
inline int Timespan::msecs() const
{
	return int((_span/Milliseconds) % 1000);
}

	
inline cxxtools::int64_t Timespan::totalMSecs() const
{
	return _span/Milliseconds;
}

	
inline int Timespan::microseconds() const
{
	return int(_span % 1000);
}


inline int Timespan::useconds() const
{
	return int(_span % 1000000);
}

	
inline cxxtools::int64_t Timespan::totalMicroseconds() const
{
	return _span;
}


inline bool Timespan::operator == (const Timespan& ts) const
{
	return _span == ts._span;
}


inline bool Timespan::operator != (const Timespan& ts) const
{
	return _span != ts._span;
}


inline bool Timespan::operator >  (const Timespan& ts) const
{
	return _span > ts._span;
}


inline bool Timespan::operator >= (const Timespan& ts) const
{
	return _span >= ts._span;
}


inline bool Timespan::operator <  (const Timespan& ts) const
{
	return _span < ts._span;
}


inline bool Timespan::operator <= (const Timespan& ts) const
{
	return _span <= ts._span;
}


inline bool Timespan::operator == (cxxtools::int64_t microseconds) const
{
	return _span == microseconds;
}


inline bool Timespan::operator != (cxxtools::int64_t microseconds) const
{
	return _span != microseconds;
}


inline bool Timespan::operator >  (cxxtools::int64_t microseconds) const
{
	return _span > microseconds;
}


inline bool Timespan::operator >= (cxxtools::int64_t microseconds) const
{
	return _span >= microseconds;
}


inline bool Timespan::operator <  (cxxtools::int64_t microseconds) const
{
	return _span < microseconds;
}


inline bool Timespan::operator <= (cxxtools::int64_t microseconds) const
{
	return _span <= microseconds;
}


inline void swap(Timespan& s1, Timespan& s2)
{
	s1.swap(s2);
}


} // namespace Poco


#endif // Foundation_Timespan_INCLUDED
