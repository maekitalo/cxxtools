/*
 * Copyright (C) 2018 by Tommi Maekitalo
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
#ifndef CXXTOOLS_TZ_H
#define CXXTOOLS_TZ_H

#include <cxxtools/datetime.h>
#include <stdexcept>
#include <string>
#include <iosfwd>

namespace cxxtools
{

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

class TzError : public std::runtime_error
{
public:
    explicit TzError(const std::string& msg)
        : std::runtime_error(msg)
        { }
};

class TzInvalidTimeZoneFile : public TzError
{
public:
    explicit TzInvalidTimeZoneFile(const std::string& msg = "invalid timezone file")
        : TzError(msg)
        { }
};

class TzAmbiguousLocalTime : public TzError
{
    DateTime _dt;

public:
    explicit TzAmbiguousLocalTime(const DateTime& dt)
        : TzError("ambiguous local time <" + dt.toString() + '>'),
          _dt(dt)
        { }

    const DateTime& which() const  { return _dt; }
};

class TzInvalidLocalTime : public TzError
{
    DateTime _dt;

public:
    explicit TzInvalidLocalTime(const DateTime& dt)
        : TzError("invalid local time <" + dt.toString() + '>'),
          _dt(dt)
        { }

    const DateTime& which() const  { return _dt; }
};

class TzDateTime;

/**
 * Time zone class.
 *
 * This class reads the iana time zone database of the system.
 *
 * It allows converting a date time in UTC to a time zoned date time.
 *
 * Example:
 * \code
 *     Tz tz("Pacific/Nauru");
 *     std::cout << "the current time in Nauru is " << tz.toLocal().toString() << '\n';
 * \endcode
 *
 * The time zones are cached so that instantiating the class
 * multiple times with the same time zone is no problem.
 *
 */
class Tz
{
    friend class TzDateTime;

    class Impl;
    Impl* _impl;

public:
    /// Returns the current time zone of the system.
    static std::string currentZone();

    /// Instantiates a time zone object.
    /// The time zone data is read from the file system and cached.
    explicit Tz(const std::string& timeZone = currentZone());

    Tz(const Tz& tz)
        : _impl(tz._impl)
    { }

    Tz& operator=(const Tz& tz)
    {
        _impl = tz._impl;
        return *this;
    }

    /// Outputs information about time zone for testing purpose.
    void dump(std::ostream& out) const;

    /// Returns the name of the time zone.
    const std::string& name() const;

    /// Interprets the specified date time as UTC and returns the time zoned date and time.
    TzDateTime toLocal(const UtcDateTime& dt) const;

    /// Returns the first UTC time witch matches the specified date time in the time zone.
    UtcDateTime toUtc(const LocalDateTime& dt) const;

    /// Returns the UTC time witch matches the specified date time in the time zone.
    /// When the result is not unique, the `early` flag specifies if the earlier or later
    /// time should be returned.
    UtcDateTime toUtc(const LocalDateTime& dt, bool early) const;

    UtcDateTime previousChange(const DateTime& gmtDt, bool local = true) const;
    UtcDateTime nextChange(const DateTime& gmtDt, bool local = true) const;

    Timespan offset(const UtcDateTime& gmtDt) const;
};

/**
 * The class extends DateTime with time zone information.
 */
class TzDateTime : public LocalDateTime
{
    std::string _tzName;
    Timespan _gmtoff;
    bool _isdst;
    uint32_t _leapSeconds;

public:
    /// Initializes the object with the specified attributes.
    TzDateTime(const DateTime& dt, const std::string& tzName, Timespan gmtoff, bool isdst, uint32_t leapSeconds = 0)
        : LocalDateTime(dt),
          _tzName(tzName),
          _gmtoff(gmtoff),
          _isdst(isdst),
          _leapSeconds(leapSeconds)
          { }

    const std::string& tzName() const   { return _tzName; }
    Timespan gmtoff() const             { return _gmtoff; }
    bool isdst() const                  { return _isdst; }
    uint32_t leapSeconds() const        { return _leapSeconds; }
};

}

#endif
