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
#include <cxxtools/config.h>

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

        Timespan operator-() const
        { return Timespan(-_span); }

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

    The template parameter specifies, which unit is returned. It is the divisor
    needed to convert a number of microseconds to the requested unit. The
    template class is normally not used directly but the typedefs
    cxxtools::Microseconds, cxxtools::Milliseconds, cxxtools::Seconds,
    cxxtools::Minutes, cxxtools::Hours and cxxtools::Days are used.

 */
template <int64_t Resolution>
class WeakTimespan : public Timespan
{
    public:
        WeakTimespan()
            : Timespan()
        { }

        WeakTimespan(short units)
            : Timespan(units * Resolution)
        { }

        WeakTimespan(unsigned short units)
            : Timespan(units * Resolution)
        { }

        WeakTimespan(int units)
            : Timespan(units * Resolution)
        { }

        WeakTimespan(unsigned int units)
            : Timespan(units * Resolution)
        { }

        WeakTimespan(long units)
            : Timespan(units * Resolution)
        { }

        WeakTimespan(unsigned long units)
            : Timespan(units * Resolution)
        { }

#ifdef HAVE_LONG_LONG
        WeakTimespan(long long units)
            : Timespan(units * Resolution)
        { }
#endif

#ifdef HAVE_UNSIGNED_LONG_LONG
        WeakTimespan(unsigned long long units)
            : Timespan(units * Resolution)
        { }
#endif

        WeakTimespan(float units)
            : Timespan(units * Resolution)
        { }

        WeakTimespan(double units)
            : Timespan(units * Resolution)
        { }

        WeakTimespan(long double units)
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

        int64_t ceil() const
        {
          int64_t r = totalUSecs();
          return r % Resolution == 0 || r < 0 ? r / Resolution
                                              : r / Resolution + 1;
        }
};

/** @brief A WeakTimespan<1> specializes a WeakTimespan for microseconds.

    Since the Timespan holds the total number of microseconds, it can be returned
    as a int64_t instead of double to prevent conversion.
 */
template <>
class WeakTimespan<1> : public Timespan
{
    public:
        WeakTimespan()
            : Timespan()
        { }

        WeakTimespan(short units)
            : Timespan(units)
        { }

        WeakTimespan(unsigned short units)
            : Timespan(units)
        { }

        WeakTimespan(int units)
            : Timespan(units)
        { }

        WeakTimespan(unsigned int units)
            : Timespan(units)
        { }

        WeakTimespan(long units)
            : Timespan(units)
        { }

        WeakTimespan(unsigned long units)
            : Timespan(units)
        { }

#ifdef HAVE_LONG_LONG
        WeakTimespan(long long units)
            : Timespan(units)
        { }
#endif

#ifdef HAVE_UNSIGNED_LONG_LONG
        WeakTimespan(unsigned long long units)
            : Timespan(units)
        { }
#endif

        WeakTimespan(float units)
            : Timespan(units)
        { }

        WeakTimespan(double units)
            : Timespan(units)
        { }

        WeakTimespan(long double units)
            : Timespan(units)
        { }

        WeakTimespan(const Timespan& ts)
            : Timespan(ts)
        { }

        operator int64_t() const
        { return totalUSecs(); }

};

/// @cond internal
namespace tshelper
{
    void get(std::istream& in, Timespan& ts, uint64_t res);
}
/// @endcond

/**
    The typedefs makes specifying a timespan easy and readable.

    Examples:
    @code
      // for this example we assume these functions:
      void foo(Milliseconds t);  // a function expecting a timespan - default unit is milliseconds
      Milliseconds foo();        // a function returning a timespan - default is to return milliseconds

      // specify 5 milliseconds:
      cxxtools::Timespan t = Milliseconds(5);

      // specify half second:
      cxxtools::Timespan halfSecond = Seconds(0.5);

      // get a timespan value in seconds:
      cxxtools::Timespan someTimespan = foo();
      double numberOfSeconds = Seconds(someTimespan);

      // or shorter:
      double numberOfSeconds(Seconds(foo());

      // pass a timespan to a function:
      foo(500);                     // since foo expects a cxxtools::Milliseconds, the default is
                                    // to convert a number to milliseconds timespan
      foo(cxxtools::Seconds(0.5));  // Seconds is a Timespan, which can be converted to Milliseconds
                                    // so that foo is here called with 500 milliseconds

      // get the number of milliseconds or some other unit in a timespan:
      cxxtools::Timespan someTimespan = foo();
      double numberOfMilliseconds = cxxtools::Milliseconds(someTimespan);
      double numberOfSeconds = cxxtools::Seconds(someTimespan);
      double numberOfDays = cxxtools::Days(someTimespan);

    @endcode
 */
typedef WeakTimespan<1>                            Microseconds;
typedef WeakTimespan<int64_t(1000)>                Milliseconds;
typedef WeakTimespan<int64_t(1000)*1000>           Seconds;
typedef WeakTimespan<int64_t(1000)*1000*60>        Minutes;
typedef WeakTimespan<int64_t(1000)*1000*60*60>     Hours;
typedef WeakTimespan<int64_t(1000)*1000*60*60*24>  Days;

inline Timespan operator * (const Timespan& d, double fac)
{
    return Timespan(d.totalUSecs() * fac);
}

inline Timespan operator * (double fac, const Timespan& d)
{
    return Timespan(d.totalUSecs() * fac);
}

inline Timespan operator / (const Timespan& d, double fac)
{
    return Timespan(d.totalUSecs() / fac);
}

/// outputs timespan as number of seconds with the suffix 's'
std::ostream& operator<< (std::ostream& out, const Timespan& ts);

/// outputs timespan as number of microseconds with the suffix 'us'
std::ostream& operator<< (std::ostream& out, const Microseconds& ts);

/// outputs timespan as number of milliseconds with the suffix 'ms'
std::ostream& operator<< (std::ostream& out, const Milliseconds& ts);

/// outputs timespan as number of seconds with the suffix 's'
std::ostream& operator<< (std::ostream& out, const Seconds& ts);

/// outputs timespan as number of minutes with the suffix 'min'
std::ostream& operator<< (std::ostream& out, const Minutes& ts);

/// outputs timespan as number of hours with the suffix 'h'
std::ostream& operator<< (std::ostream& out, const Hours& ts);

/// outputs timespan as number of days with the suffix 'd'
std::ostream& operator<< (std::ostream& out, const Days& ts);

/// reads a whole number from stream and creates a timespan with the number of microseconds
std::istream& operator>> (std::istream& in, Timespan& ts);

/// reads a whole number from stream and creates a timespan with the number of microseconds
std::istream& operator>> (std::istream& in, Microseconds& ts);

/// reads a decimal number from stream and creates a timespan with the number of milliseconds
std::istream& operator>> (std::istream& in, Milliseconds& ts);

/// reads a decimal number from stream and creates a timespan with the number of seconds
std::istream& operator>> (std::istream& in, Seconds& ts);

/// reads a decimal number from stream and creates a timespan with the number of minutes
std::istream& operator>> (std::istream& in, Minutes& ts);

/// reads a decimal number from stream and creates a timespan with the number of hours
std::istream& operator>> (std::istream& in, Hours& ts);

/// reads a decimal number from stream and creates a timespan with the number of days
std::istream& operator>> (std::istream& in, Days& ts);

template <uint64_t Resolution>
std::istream& operator>> (std::istream& in, WeakTimespan<Resolution>& ts)
{
    tshelper::get(in, ts, Resolution);
    return in;
}

void operator >>=(const SerializationInfo& si, Timespan& timespan);

void operator <<=(SerializationInfo& si, const Timespan& timespan);

void operator <<=(SerializationInfo& si, const Microseconds& timespan);

void operator <<=(SerializationInfo& si, const Milliseconds& timespan);

void operator <<=(SerializationInfo& si, const Seconds& timespan);

void operator <<=(SerializationInfo& si, const Minutes& timespan);

void operator <<=(SerializationInfo& si, const Hours& timespan);

void operator <<=(SerializationInfo& si, const Days& timespan);

} // namespace cxxtools

#endif
