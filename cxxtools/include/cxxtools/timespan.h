#ifndef cxxtools_Timespan_h
#define cxxtools_Timespan_h

#include <cxxtools/types.h>
#include <algorithm>
#include <iosfwd>

namespace cxxtools {

/** @brief  Represents time spans up to microsecond resolution.
    @ingroup DateTime
*/
class Timespan
{
    public:
        //! @brief Creates a zero Timespan.
        Timespan()
        : _span(0)
        {}

        //! @brief Creates a Timespan.
        Timespan(cxxtools::int64_t microseconds)
        : _span(microseconds)
        { }

        /** @brief Creates a Timespan.
            Useful for creating a Timespan from a struct timeval.
        */
        Timespan(long seconds, long microseconds)
        : _span(cxxtools::int64_t(seconds)*Seconds + microseconds)
        {
        }
        //! @brief Creates a Timespan.
        Timespan(int days, int hours, int minutes, int seconds, int microseconds);

        //! @brief Creates a Timespan from another one.
        Timespan(const Timespan& timespan);

        //! @brief Destroys the Timespan.
        ~Timespan()
        {}

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
        cxxtools::int64_t toUSecs() const;

        //! @brief The number of microseconds in a millisecond.
        //static const cxxtools::int64_t Milliseconds;

        //! @brief The number of microseconds in a second.
        //static const cxxtools::int64_t Seconds;

        //! @brief The number of microseconds in a minute.
        //static const cxxtools::int64_t Minutes;

        //! @brief The number of microseconds in a hour.
        //static const cxxtools::int64_t Hours;

        //! @brief The number of microseconds in a day.
        //static const cxxtools::int64_t Days;

        static const cxxtools::int64_t Milliseconds = 1000;
        static const cxxtools::int64_t Seconds      = 1000 * Timespan::Milliseconds;
        static const cxxtools::int64_t Minutes      =   60 * Timespan::Seconds;
        static const cxxtools::int64_t Hours        =   60 * Timespan::Minutes;
        static const cxxtools::int64_t Days         =   24 * Timespan::Hours;

        //! @brief returns the current time as a timespan value.
        static Timespan gettimeofday();

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


inline cxxtools::int64_t Timespan::toUSecs() const
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


inline Timespan::Timespan(int days, int hours, int minutes, int seconds, int microseconds)
: _span( cxxtools::int64_t(microseconds) +
         cxxtools::int64_t(seconds)*Seconds +
         cxxtools::int64_t(minutes)*Minutes +
         cxxtools::int64_t(hours)*Hours +
         cxxtools::int64_t(days)*Days )
{
}


inline Timespan::Timespan(const Timespan& timespan)
: _span(timespan._span)
{
}


inline Timespan& Timespan::operator=(const Timespan& timespan)
{
    _span = timespan._span;
    return *this;
}


inline Timespan& Timespan::operator=(cxxtools::int64_t microseconds)
{
    _span = microseconds;
    return *this;
}


inline Timespan& Timespan::set(int days, int hours, int minutes, int seconds, int microseconds)
{
    _span = cxxtools::int64_t(microseconds) +
            cxxtools::int64_t(seconds)*Seconds +
            cxxtools::int64_t(minutes)*Minutes +
            cxxtools::int64_t(hours)*Hours +
            cxxtools::int64_t(days)*Days;
    return *this;
}


inline Timespan& Timespan::set(long seconds, long microseconds)
{
    _span = cxxtools::int64_t(seconds)*Seconds + cxxtools::int64_t(microseconds);
    return *this;
}


inline void Timespan::swap(Timespan& timespan)
{
    std::swap(_span, timespan._span);
}


inline Timespan Timespan::operator + (const Timespan& d) const
{
    return Timespan(_span + d._span);
}


inline Timespan Timespan::operator - (const Timespan& d) const
{
    return Timespan(_span - d._span);
}


inline Timespan& Timespan::operator += (const Timespan& d)
{
    _span += d._span;
    return *this;
}


inline Timespan& Timespan::operator -= (const Timespan& d)
{
    _span -= d._span;
    return *this;
}


inline Timespan Timespan::operator + (cxxtools::int64_t microseconds) const
{
    return Timespan(_span + microseconds);
}


inline Timespan Timespan::operator - (cxxtools::int64_t microseconds) const
{
    return Timespan(_span - microseconds);
}


inline Timespan& Timespan::operator += (cxxtools::int64_t microseconds)
{
    _span += microseconds;
    return *this;
}


inline Timespan& Timespan::operator -= (cxxtools::int64_t microseconds)
{
    _span -= microseconds;
    return *this;
}

std::ostream& operator<< (std::ostream& out, const Timespan& ht);

} // namespace cxxtools

#endif 
