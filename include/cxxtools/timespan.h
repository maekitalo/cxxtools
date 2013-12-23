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
#ifndef cxxtools_Timespan_h
#define cxxtools_Timespan_h

#include <stdint.h>
#include <iosfwd>

namespace cxxtools {

class SerializationInfo;

/** @brief  Represents time spans up to microsecond resolution.
    @ingroup DateTime
*/
class Timespan
{
    public:
        //! @brief Creates a Timespan.
        explicit Timespan(int64_t microseconds = 0)
        : _span(microseconds)
        { }

        /** @brief Creates a Timespan.
            Useful for creating a Timespan from a struct timeval.
        */
        Timespan(long seconds, long microseconds)
        : _span(int64_t(seconds)*1000*1000 + microseconds)
        {
        }
        //! @brief Creates a Timespan.
        Timespan(int days, int hours, int minutes, int seconds, int microseconds)
        : _span( int64_t(microseconds) +
                 int64_t(seconds)*1000*1000 +
                 int64_t(minutes)*1000*1000*60 +
                 int64_t(hours)*1000*1000*60*60 +
                 int64_t(days)*1000*1000*60*60*24 )
        {
        }

        bool operator==(const Timespan& ts) const
        { return _span == ts._span; }

        bool operator!=(const Timespan& ts) const
        { return _span != ts._span; }

        bool operator>(const Timespan& ts) const
        { return _span > ts._span; }

        bool operator>=(const Timespan& ts) const
        { return _span >= ts._span; }

        bool operator<(const Timespan& ts) const
        { return _span < ts._span; }

        bool operator<=(const Timespan& ts) const
        { return _span <= ts._span; }

        Timespan operator+(const Timespan& d) const
        { return Timespan(_span + d._span); }

        Timespan operator-(const Timespan& d) const
        { return Timespan(_span - d._span); }

        Timespan& operator+=(const Timespan& d)
        { _span += d._span; return *this; }

        Timespan& operator-=(const Timespan& d)
        { _span -= d._span; return *this; }

        //! @brief Returns the total number of hours.
        double totalDays() const
        { return double(_span) / 1000 / 1000 / 60 / 60 / 24; }

        //! @brief Returns the total number of hours.
        double totalHours() const
        { return double(_span) / 1000 / 1000 / 60 / 60; }

        //! @brief Returns the total number of minutes.
        double totalMinutes() const
        { return double(_span) / 1000 / 1000 / 60; }

        //! @brief Returns the total number of seconds.
        double totalSeconds() const
        { return double(_span) / 1000 / 1000; }

        //! @brief Returns the total number of milliseconds.
        double totalMSecs() const
        { return double(_span) / 1000; }

        //! @brief Returns the total number of microseconds.
        int64_t totalUSecs() const
        { return _span; }

        //! @brief returns the current time as a timespan value.
        static Timespan gettimeofday();

    private:
        int64_t _span;
};

/** @brief A WeakTimespan extends a Timespan with a implicit conversion to and from number.

    The template parameter specifies, which unit is returned. It is the devisor
    needed to convert a number of microseconds to the requested unit.
 */
template <uint64_t Resolution>
class WeakTimespan : public Timespan
{
    public:
        WeakTimespan(double units = 0)
            : Timespan(units * Resolution)
        { }

        WeakTimespan(const Timespan& ts)
            : Timespan(ts)
        { }

        WeakTimespan(long seconds, long microseconds)
            : Timespan(seconds, microseconds)
        { }

        WeakTimespan(int days, int hours, int minutes, int seconds, int microseconds)
            : Timespan(days, hours, minutes, seconds, microseconds)
        { }

        operator double() const
        { return totalUSecs() / static_cast<double>(Resolution); }

};

/** @brief A WeakTimespan<1> specializes a WeakTimespan for microseconds.

    Since the Timespan holds the total number of microseconds, it can be returned
    as a int64_t instead of double to prevent conversion.
 */
template <>
class WeakTimespan<1> : public Timespan
{
    public:
        WeakTimespan(int64_t microseconds = 0)
            : Timespan(microseconds)
        { }

        WeakTimespan(const Timespan& ts)
            : Timespan(ts)
        { }

        operator uint64_t() const
        { return totalUSecs(); }

};

/**
    The typedefs makes specifying a timespan easy and readable.

    Examples:
    @code
      // specify 5 milliseconds:
      cxxtools::Timespan t = Milliseconds(5);

      // specify half second:
      cxxtools::Timespan halfSecond = Seconds(0.5);

      // get a timespan value in seconds:
      cxxtools::Timespan someTimespan = foo();
      double numberOfSeconds = Seconds(someTimespan);

      // get a timespan value in whole milliseconds:
      cxxtools::Timespan someTimespan = foo();
      int numberOfMilliseconds = Millieconds(someTimespan);
    @endcode
 */
typedef WeakTimespan<1>                             Microseconds;
typedef WeakTimespan<uint64_t(1000)>                Milliseconds;
typedef WeakTimespan<uint64_t(1000)*1000>           Seconds;
typedef WeakTimespan<uint64_t(1000)*1000*60>        Minutes;
typedef WeakTimespan<uint64_t(1000)*1000*60*60>     Hours;
typedef WeakTimespan<uint64_t(1000)*1000*60*60*24>  Days;

inline Timespan operator * (const Timespan& d, double fac)
{
    return Timespan(d.totalUSecs() * fac);
}

inline Timespan operator * (double fac, const Timespan& d)
{
    return Timespan(d.totalUSecs() * fac);
}

std::ostream& operator<< (std::ostream& out, const Timespan& ht);

void operator >>=(const SerializationInfo& si, Timespan& timespan);

void operator <<=(SerializationInfo& si, const Timespan& timespan);

} // namespace cxxtools

#endif
