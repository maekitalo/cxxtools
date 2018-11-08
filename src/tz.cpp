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
#include <cxxtools/tz.h>

#include <cxxtools/log.h>
#include <cxxtools/datetime.h>
#include <cxxtools/mutex.h>
#include <cxxtools/smartptr.h>
#include <cxxtools/systemerror.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <map>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <limits.h>

log_define("cxxtools.tz")

namespace cxxtools
{

class Tz::Impl : public RefCounted
{
    friend class Tz;
    friend class TzDateTime;

    typedef std::map<std::string, cxxtools::SmartPtr<Impl> > TimeZones;
    static TimeZones timeZones;
    static cxxtools::Mutex mutex;

    struct TzHead
    {
        char version;
        uint32_t ttisgmtcnt;
        uint32_t ttisstdcnt;
        uint32_t leapcnt;
        uint32_t timecnt;
        uint32_t typecnt;
        uint32_t charcnt;
    };

    struct TtInfo
    {
        int32_t gmtoff;
        char isdst;
        int abbreviationIndex;
    };

    struct LeapInfo
    {
        time_t transitionTime;
        int32_t corrections;
    };

    struct Transition
    {
        time_t transitionTime;
        uint8_t ttIndex;
    };

    std::string _name;
    TzHead head;
    std::vector<Transition> transitions;
    std::vector<TtInfo> ttInfos;
    std::string abbreviations;
    std::vector<LeapInfo> leapInfos;

    template <typename IntType>
    static void get(std::istream& in, IntType& ret)
    {
        char ch;
        ret = 0;
        for (size_t i = 0; i < sizeof(IntType); ++i)
        {
            in.get(ch);
            ret = ((ret << 8) | static_cast<unsigned char>(ch));
        }
    }

    static void check(std::istream& in)
    {
        if (!in)
            throw TzInvalidTimeZoneFile();
    }

    template <typename IntType> void loadData(std::istream& in)
    {
        // trasition times
        log_debug("read transition times");

        transitions.resize(head.timecnt);
        for (unsigned i = 0; i < head.timecnt; ++i)
        {
            IntType t;
            get(in, t);
            transitions[i].transitionTime = t;
        }

        check(in);

        // indices
        log_debug("read indices");
        for (unsigned i = 0; i < head.timecnt; ++i)
            transitions[i].ttIndex = in.get();
        check(in);

        for (unsigned i = 0; i < head.timecnt; ++i)
        {
            if (transitions[i].ttIndex >= head.typecnt)
                throw TzInvalidTimeZoneFile();

            log_debug("transitions[" << i << "]={transitionTime: \"" << cxxtools::DateTime::fromMSecsSinceEpoch(transitions[i].transitionTime).toString() << "\", ttIndex: " << static_cast<int>(transitions[i].ttIndex) << '}');
        }

        // ttinfo
        log_debug("read ttinfo");
        ttInfos.resize(head.typecnt);
        for (unsigned i = 0; i < head.typecnt; ++i)
        {
            get(in, ttInfos[i].gmtoff);
            in.get(ttInfos[i].isdst);
            ttInfos[i].abbreviationIndex = in.get();
        }
        check(in);
        for (unsigned i = 0; i < head.typecnt; ++i)
            if (ttInfos[i].abbreviationIndex > static_cast<int>(head.charcnt))
                throw TzInvalidTimeZoneFile();

        log_debug("read abbreviations");
        abbreviations.reserve(head.charcnt);
        for (unsigned i = 0; i < head.charcnt; ++i)
        {
            char ch;
            in.get(ch);
            abbreviations.push_back(ch);
        }
        check(in);

        // verify

        for (unsigned i = 0; i < head.typecnt; ++i)
            log_debug("ttInfos[" << i << "]={gmtoff: " << ttInfos[i].gmtoff << ", isdst: " << (ttInfos[i].isdst ? '1' : '0') << ", abbreviationIndex: " << ttInfos[i].abbreviationIndex << ", abbreviation: " << (abbreviations.data() + ttInfos[i].abbreviationIndex) << '}');

        log_debug("read leapInfos");
        leapInfos.resize(head.leapcnt);
        for (unsigned i = 0; i < head.leapcnt; ++i)
        {
            get(in, leapInfos[i].transitionTime);
            get(in, leapInfos[i].corrections);
        }
        for (unsigned i = 0; i < leapInfos.size(); ++i)
            log_debug("leapInfos[" << i << "]={transitionTime: " << leapInfos[i].transitionTime << ", corrections: " << leapInfos[i].corrections << '}');

        log_debug("head finished");
    }

    const char* abbreviation(int index) const
    { return abbreviations.data() + index; }

public:
    explicit Impl(std::istream& in, const std::string& name);

    const std::string& name() const  { return _name; }
    void dump(std::ostream& out) const;
};

Tz::Impl::Impl(std::istream& in, const std::string& name)
    : _name(name)
{
    log_debug("read magic");

    char magic[4];
    in.read(magic, 4);
    log_debug("magic=" << std::string(magic, 4));

    check(in);
    if (memcmp(magic, "TZif", 4) != 0)
        throw std::runtime_error("invalid timezone file: bad magic");

    in.get(head.version);
    log_debug_if(head.version, "version: " << head.version);
    log_debug_if(!head.version, "no version");

    in.ignore(15);
    get(in, head.ttisgmtcnt);
    get(in, head.ttisstdcnt);
    get(in, head.leapcnt);
    get(in, head.timecnt);
    get(in, head.typecnt);
    get(in, head.charcnt);
    log_debug("counters read");

    check(in);
    log_debug("ttisgmtcnt: " << head.ttisgmtcnt << "\t"
           "ttisstdcnt: " << head.ttisstdcnt << "\t"
           "leapcnt: " << head.leapcnt << "\t"
           "timecnt: " << head.timecnt << "\t"
           "typecnt: " << head.typecnt << "\t"
           "charcnt: " << head.charcnt);

    if (head.version == 0)
    {
        log_debug("loadData 32");
        loadData<int32_t>(in);
    }
    else
    {
        in.ignore((4+1)*head.timecnt + 6*head.typecnt + head.charcnt + 8*head.leapcnt +
                   head.ttisstdcnt + head.ttisgmtcnt + (4+1+15));
        get(in, head.ttisgmtcnt);
        get(in, head.ttisstdcnt);
        get(in, head.leapcnt);
        get(in, head.timecnt);
        get(in, head.typecnt);
        get(in, head.charcnt);

        check(in);

        log_debug("counters read 2");
        log_debug("ttisgmtcnt: " << head.ttisgmtcnt << "\t"
               "ttisstdcnt: " << head.ttisstdcnt << "\t"
               "leapcnt: " << head.leapcnt << "\t"
               "timecnt: " << head.timecnt << "\t"
               "typecnt: " << head.typecnt << "\t"
               "charcnt: " << head.charcnt);

        log_debug("loadData 64");
        loadData<int64_t>(in);
    }
}

void Tz::Impl::dump(std::ostream& out) const
{
    for (unsigned i = 0; i < head.timecnt; ++i)
    {
        const TtInfo& ttinfo = ttInfos[transitions[i].ttIndex];
        out << i << ' ' << cxxtools::DateTime::fromMSecsSinceEpoch(cxxtools::Seconds(transitions[i].transitionTime)).toString() << " UTC =\t"
                  << cxxtools::DateTime::fromMSecsSinceEpoch(cxxtools::Seconds(transitions[i].transitionTime + ttinfo.gmtoff)).toString()
                  << " " << abbreviation(ttinfo.abbreviationIndex) << "\tisdst=" << (ttinfo.isdst ? '1' : '0') << " gmtoff=" << ttinfo.gmtoff << '\n';
    }

    for (unsigned i = 0; i < leapInfos.size(); ++i)
    {
        out << cxxtools::DateTime::fromMSecsSinceEpoch(cxxtools::Seconds(leapInfos[i].transitionTime)).toString() << " leap corrections: " << leapInfos[i].corrections << '\n';
    }
}

////////////////////////////////////////////////////////////////////////
// class Tz
//
const char* tz_dir = "/usr/share/zoneinfo";

Tz::Impl::TimeZones Tz::Impl::timeZones;
cxxtools::Mutex Tz::Impl::mutex;

Tz::Tz(const std::string& timeZone)
    : _impl(0)
{
    cxxtools::MutexLock lock(Impl::mutex);

    std::string timeZone2 = timeZone;
    if (timeZone2.empty())
        timeZone2 = currentZone();

    auto it = Impl::timeZones.find(timeZone2);
    if (it != Impl::timeZones.end())
    {
        log_debug("take cached");
        _impl = it->second.getPointer();
    }
    else
    {
        const char* TZDIR = ::getenv("TZDIR");
        std::string fname = TZDIR ? TZDIR : tz_dir;
        fname += '/';
        fname += timeZone2;
        std::ifstream in(fname);

        if (!in)
            throw TzInvalidTimeZoneFile("failed to open timezone file \"" + fname + '"');

        cxxtools::SmartPtr<Impl> impl(new Impl(in, timeZone2));
        _impl = impl.getPointer();

        Impl::timeZones.insert(
            Impl::TimeZones::value_type(timeZone2,
                                        std::move(impl)));
    }
}

void Tz::dump(std::ostream& out) const
{
    _impl->dump(out);
}

const std::string& Tz::name() const
{
    return _impl->name();
}

TzDateTime Tz::toLocal(const UtcDateTime& dt) const
{
    time_t t = static_cast<time_t>(dt.msecsSinceEpoch().totalSeconds());

    uint8_t ttIndex = 0;
    for (unsigned i = 0; i < _impl->transitions.size(); ++i)
    {
        if (_impl->transitions[i].transitionTime > t)
            break;
        ttIndex = _impl->transitions[i].ttIndex;
    }

    const auto& tt = _impl->ttInfos[ttIndex];

    std::string tzName = _impl->abbreviations.data() + tt.abbreviationIndex;
    cxxtools::Timespan gmtoff = cxxtools::Seconds(tt.gmtoff);
    bool isdst = tt.isdst;

    uint32_t leapSeconds = 0;
    for (unsigned i = 0; i < _impl->leapInfos.size(); ++i)
    {
        if (_impl->leapInfos[i].transitionTime > t)
            break;

        leapSeconds = _impl->leapInfos[i].corrections;
    }

    return TzDateTime(dt + gmtoff, tzName, gmtoff, isdst, leapSeconds);
}

static std::string timeT2s(time_t t)
{
    return cxxtools::DateTime::fromMSecsSinceEpoch(cxxtools::Seconds(t)).toString();
}

UtcDateTime Tz::toUtc(const LocalDateTime& dt) const
{
    log_debug("toUtc(" << dt.toString() << ')');

    time_t t = static_cast<time_t>(dt.msecsSinceEpoch().totalSeconds());
    unsigned i;
    for (i = 0; i < _impl->transitions.size() - 1; ++i)
    {
        const auto& tt = _impl->ttInfos[_impl->transitions[i].ttIndex];
        if (_impl->transitions[i + 1].transitionTime > t - tt.gmtoff)
        {
            // This is the first transition range, which is still valid.

            log_debug(i << " transitionTime: " << timeT2s(_impl->transitions[i + 1].transitionTime) << ' ' << (tt.isdst ? "dst" : "nodst"));

            if (_impl->transitions[i].transitionTime > t - tt.gmtoff)
                throw TzInvalidLocalTime(dt);

            const auto& tt2 = _impl->ttInfos[_impl->transitions[i + 1].ttIndex];

            // Check whether the next range is already valid
            log_debug(timeT2s(_impl->transitions[i + 1].transitionTime) << ", " << timeT2s(t) << ", " << tt2.gmtoff << ", " << timeT2s(t - tt2.gmtoff));
            if (_impl->transitions[i + 1].transitionTime <= t - tt2.gmtoff)
            {
                log_debug("next transition is also valid; gmtoff=" << tt2.gmtoff);
                throw TzAmbiguousLocalTime(dt);
            }

            log_debug("next transition is not valid; gmtoff=" << tt.gmtoff);
            return UtcDateTime(dt - cxxtools::Seconds(tt.gmtoff));
        }
    }

    const auto& tt = _impl->ttInfos[_impl->transitions[i].ttIndex];
    return UtcDateTime(dt - cxxtools::Seconds(tt.gmtoff));
}

UtcDateTime Tz::toUtc(const LocalDateTime& dt, bool early) const
{
    log_debug("toUtc(" << dt.toString() << ", " << early << ')');

    time_t t = static_cast<time_t>(dt.msecsSinceEpoch().totalSeconds());
    unsigned i;
    for (i = 0; i < _impl->transitions.size() - 1; ++i)
    {
        const auto& tt = _impl->ttInfos[_impl->transitions[i].ttIndex];
        if (_impl->transitions[i + 1].transitionTime > t - tt.gmtoff)
        {
            // This is the first transition range, which is still valid.

            log_debug(i << " transitionTime: " << timeT2s(_impl->transitions[i + 1].transitionTime) << ' ' << (tt.isdst ? "dst" : "nodst"));

            if (_impl->transitions[i].transitionTime > t - tt.gmtoff)
                throw TzInvalidLocalTime(dt);

            if (early)
            {
                log_debug("take earliest; gmtoff=" << tt.gmtoff);
                return UtcDateTime(dt - cxxtools::Seconds(tt.gmtoff));
            }

            const auto& tt2 = _impl->ttInfos[_impl->transitions[i + 1].ttIndex];

            // Check whether the next range is already valid
            log_debug(timeT2s(_impl->transitions[i + 1].transitionTime) << ", " << timeT2s(t) << ", " << tt2.gmtoff << ", " << timeT2s(t - tt2.gmtoff));
            if (_impl->transitions[i + 1].transitionTime <= t - tt2.gmtoff)
            {
                log_debug("next transition is also valid; gmtoff=" << tt2.gmtoff);
                return UtcDateTime(dt - cxxtools::Seconds(tt2.gmtoff));
            }
            else
            {
                log_debug("next transition is not valid; gmtoff=" << tt.gmtoff);
                return UtcDateTime(dt - cxxtools::Seconds(tt.gmtoff));
            }
        }
    }

    const auto& tt = _impl->ttInfos[_impl->transitions[i].ttIndex];
    return UtcDateTime(dt - cxxtools::Seconds(tt.gmtoff));
}

UtcDateTime Tz::previousChange(const cxxtools::DateTime& dt, bool local) const
{
    log_debug("previousChange(" << dt.toString() << ')');

    time_t t = static_cast<time_t>(dt.msecsSinceEpoch().totalSeconds());
    for (unsigned i = 0; i < _impl->transitions.size() - 1; ++i)
    {
        if (_impl->transitions[i + 1].transitionTime > t)
        {
            int32_t gmtoff = local ? _impl->ttInfos[_impl->transitions[i].ttIndex].gmtoff : 0;
            return UtcDateTime(cxxtools::DateTime::fromMSecsSinceEpoch(cxxtools::Seconds(_impl->transitions[i].transitionTime + gmtoff)));
        }
    }

    return UtcDateTime(0, 1, 1, 0, 0, 0);
}

UtcDateTime Tz::nextChange(const cxxtools::DateTime& dt, bool local) const
{
    log_debug("nextChange(" << dt.toString() << ')');

    time_t t = static_cast<time_t>(dt.msecsSinceEpoch().totalSeconds());
    for (unsigned i = 0; i < _impl->transitions.size() - 1; ++i)
    {
        if (_impl->transitions[i + 1].transitionTime > t)
        {
            int32_t gmtoff = local ? _impl->ttInfos[_impl->transitions[i].ttIndex].gmtoff : 0;
            return UtcDateTime(cxxtools::DateTime::fromMSecsSinceEpoch(cxxtools::Seconds(_impl->transitions[i + 1].transitionTime + gmtoff)));
        }
    }

    return UtcDateTime(9999, 12, 31, 23, 59, 59, 999, 999);
}

cxxtools::Timespan Tz::offset(const UtcDateTime& gmtDt) const
{
    time_t t = static_cast<time_t>(gmtDt.msecsSinceEpoch().totalSeconds());

    uint8_t ttIndex = 0;
    for (unsigned i = 0; i < _impl->transitions.size(); ++i)
    {
        if (_impl->transitions[i].transitionTime > t)
            break;
        ttIndex = _impl->transitions[i].ttIndex;
    }

    const auto& tt = _impl->ttInfos[ttIndex];
    return cxxtools::Seconds(tt.gmtoff);
}

std::string Tz::currentZone()
{
    const char* TZ = ::getenv("TZ");
    if (TZ)
        return TZ;

    // copied from Howad Hinnant: https://github.com/HowardHinnant/date/blob/master/tz.cpp

    // On some OS's a file called /etc/localtime may
    // exist and it may be either a real file
    // containing time zone details or a symlink to such a file.
    // On MacOS and BSD Unix if this file is a symlink it
    // might resolve to a path like this:
    // "/usr/share/zoneinfo/America/Los_Angeles"
    // If it does, we try to determine the current
    // timezone from the remainder of the path by removing the prefix
    // and hoping the rest resolves to valid timezone.
    // It may not always work though. If it doesn't then an
    // exception will be thrown by local_timezone.
    // The path may also take a relative form:
    // "../usr/share/zoneinfo/America/Los_Angeles".
    struct stat sb;
    static const char timezone[] = "/etc/localtime";
    if (lstat(timezone, &sb) == 0 && S_ISLNK(sb.st_mode) && sb.st_size > 0)
    {
        using namespace std;
        string result;
        char rp[PATH_MAX];
        if (realpath(timezone, rp))
            result = string(rp);
        else
            throw SystemError("realpath");

        const size_t pos = result.find(tz_dir);
        if (pos != result.npos)
            result.erase(0, sizeof(tz_dir)+pos);
        return result;
    }
    {
    // On some versions of some linux distro's (e.g. Ubuntu),
    // the current timezone might be in the first line of
    // the /etc/timezone file.
        std::ifstream timezone_file("/etc/timezone");
        if (timezone_file.is_open())
        {
            std::string result;
            std::getline(timezone_file, result);
            if (!result.empty())
                return result;
        }
        // Fall through to try other means.
    }
    {
    // On some versions of some linux distro's (e.g. Red Hat),
    // the current timezone might be in the first line of
    // the /etc/sysconfig/clock file as:
    // ZONE="US/Eastern"
        std::ifstream timezone_file("/etc/sysconfig/clock");
        std::string result;
        while (timezone_file)
        {
            std::getline(timezone_file, result);
            auto p = result.find("ZONE=\"");
            if (p != std::string::npos)
            {
                result.erase(p, p+6);
                result.erase(result.rfind('"'));
                return result;
            }
        }
        // Fall through to try other means.
    }
    throw std::runtime_error("Could not get current timezone");
}

}
