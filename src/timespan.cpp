/*
 * Copyright (C) 2008 Tommi Maekitalo
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

#include <cxxtools/timespan.h>
#include <cxxtools/serializationinfo.h>
#include <cxxtools/serializationerror.h>
#include <cxxtools/convert.h>

#include "dateutils.h"

#include <sys/time.h>
#include <time.h>
#include <stdio.h>

#include <iostream>
#include <iterator>

namespace cxxtools
{
    std::string Timespan::toString() const
    {
        std::string ret;

        unsigned h = hours();
        unsigned m = minutes();
        unsigned s = seconds();
        unsigned mm = microseconds();
        if (h > 0)
        {
            putInt(std::back_inserter(ret), h);
            ret += ':';
            appendDn(ret, 2, m);
            ret += ':';
            appendDn(ret, 2, s);
        }
        else if (m > 0)
        {
            appendDn(ret, m < 10 ? 1 : 2, m);
            ret += ':';
            appendDn(ret, 2, s);
        }
        else
        {
            appendDn(ret, s < 10 ? 1 : 2, s);
        }

        if (mm > 0)
        {
            ret += '.';
            for (unsigned p = 6; p > 0; --p, mm /= 10)
            {
                if (mm % 10 != 0)
                {
                    appendDn(ret, p, mm);
                    break;
                }
            }
        }

        return ret;
    }

    Timespan Timespan::gettimeofday()
    {
        timeval tv;
        ::gettimeofday(&tv, 0);
        return Timespan(tv.tv_sec, tv.tv_usec);
    }

    std::ostream& operator<< (std::ostream& out, const Timespan& ts)
    {
        out << static_cast<double>(ts.totalSeconds()) << 's';
        return out;
    }

    std::ostream& operator<< (std::ostream& out, const Microseconds& ts)
    {
        out << ts.totalUSecs() << "us";
        return out;
    }

    std::ostream& operator<< (std::ostream& out, const Milliseconds& ts)
    {
        out << static_cast<double>(ts.totalMSecs()) << "ms";
        return out;
    }

    std::ostream& operator<< (std::ostream& out, const Seconds& ts)
    {
        out << static_cast<double>(ts.totalSeconds()) << 's';
        return out;
    }

    std::ostream& operator<< (std::ostream& out, const Minutes& ts)
    {
        out << static_cast<double>(ts.totalMinutes()) << "min";
        return out;
    }

    std::ostream& operator<< (std::ostream& out, const Hours& ts)
    {
        out << static_cast<double>(ts.totalHours()) << 'h';
        return out;
    }

    std::ostream& operator<< (std::ostream& out, const Days& ts)
    {
        out << static_cast<double>(ts.totalDays()) << 'd';
        return out;
    }

    namespace tshelper
    {
        void get(std::istream& in, Timespan& ts, uint64_t res)
        {
            double value;
            in >> value;
            if (in)
            {
                int ch = in.peek();
                if (in)
                {
                    switch (std::istream::traits_type::to_char_type(ch))
                    {
                        case 'u':
                            in.get();
                            ts = Microseconds(value);
                            return;

                        case 'm':
                        {
                            in.get();
                            ch = in.peek();
                            if (in)
                            {
                                switch (std::istream::traits_type::to_char_type(ch))
                                {
                                    case 's':
                                        in.get();
                                        ts = Milliseconds(value);
                                        return;

                                    case 'i':
                                        in.get();
                                        ts = Minutes(value);
                                        return;
                                }
                            }

                            in.clear();
                            ts = Minutes(value);
                            return;
                        }

                        case 's':
                            in.get();
                            ts = Seconds(value);
                            return;

                        case 'h':
                            in.get();
                            ts = Hours(value);
                            return;

                        case 'd':
                            in.get();
                            ts = Days(value);
                            return;
                    }
                }

                in.clear();

                ts = Timespan(value * res);
            }
        }

        uint64_t factor(const cxxtools::SerializationInfo& si, uint64_t res)
        {
            if (si.typeName() == "microseconds")
                return 1;
            else if (si.typeName() == "milliseconds")
                return 1000l;
            else if (si.typeName() == "seconds")
                return 1000l*1000l;
            else if (si.typeName() == "minutes")
                return 1000l*1000l*60l;
            else if (si.typeName() == "hours")
                return static_cast<uint64_t>(1000)*1000l*60l*60l;
            else if (si.typeName() == "days")
                return static_cast<uint64_t>(1000)*1000l*60l*60l*24l;
            else
                return res;
        }

        void get(const SerializationInfo& si, Timespan& timespan, uint64_t res)
        {
            if (si.isInt())
            {
                int64_t value;
                si >>= value;
                timespan = Timespan(value);
            }
            else if (si.isFloat())
            {
                float value;
                si >>= value;
                timespan = Timespan(value * factor(si, res));
            }
            else if (si.isDouble())
            {
                double value;
                si >>= value;
                timespan = Timespan(value * factor(si, res));
            }
            else if (si.isLongDouble())
            {
                long double value;
                si >>= value;
                timespan = Timespan(value * factor(si, res));
            }
            else
            {
                std::string stringValue;
                si >>= stringValue;

                bool ok;
                double floatValue;
                std::string::const_iterator end = getFloat(stringValue.begin(), stringValue.end(), ok, floatValue);

                if (!ok)
                {
                    std::string msg = "failed to get timespan from value \"";
                    msg += stringValue;
                    msg += '"';
                    throw SerializationError(msg);
                }

                if (end != stringValue.end())
                {
                    std::string::size_type pos = end - stringValue.begin();
                    if (stringValue.compare(pos, 1, "u") == 0)
                        timespan = Microseconds(floatValue);
                    else if (stringValue.compare(pos, 2, "ms") == 0 || stringValue.compare(pos, 3, "mil") == 0)
                        timespan = Milliseconds(floatValue);
                    else if (stringValue.compare(pos, 1, "s") == 0)
                        timespan = Seconds(floatValue);
                    else if (stringValue.compare(pos, 1, "m") == 0)
                        timespan = Minutes(floatValue);
                    else if (stringValue.compare(pos, 1, "h") == 0)
                        timespan = Hours(floatValue);
                    else if (stringValue.compare(pos, 1, "d") == 0)
                        timespan = Days(floatValue);
                    else
                        timespan = Timespan(floatValue * res);
                }
                else if (si.typeName() == "microseconds")
                    timespan = Microseconds(floatValue);
                else if (si.typeName() == "milliseconds")
                    timespan = Milliseconds(floatValue);
                else if (si.typeName() == "seconds")
                    timespan = Seconds(floatValue);
                else if (si.typeName() == "minutes")
                    timespan = Minutes(floatValue);
                else if (si.typeName() == "hours")
                    timespan = Hours(floatValue);
                else if (si.typeName() == "days")
                    timespan = Days(floatValue);
                else
                    timespan = Timespan(floatValue * res);
            }
        }
    }

    void operator <<=(SerializationInfo& si, const Timespan& timespan)
    {
        si <<= timespan.totalUSecs();
        si.setTypeName("microseconds");
    }

    void operator <<=(SerializationInfo& si, const Microseconds& timespan)
    {
        si <<= timespan.totalUSecs();
        si.setTypeName("microseconds");
    }

    static std::string toString(int64_t number, unsigned short r)
    {
        unsigned divisor = 1;
        for (unsigned short rr = 0; rr < r; ++rr)
            divisor *= 10;

        uint64_t absnum = static_cast<uint64_t>(number >= 0 ? number : -number);
        unsigned reminder = absnum % divisor;
        char buffer[24];
        if (reminder == 0)
        {
            snprintf(buffer, 24, "%lld", (long long)(number / divisor));
            return buffer;
        }

        snprintf(buffer, 24, "%lld", (long long)(absnum / divisor));

        char bb[24];
        snprintf(bb, 24, "%u", reminder);

        std::string ret(bb);
        ret.insert(0, r - ret.size(), '0');
        ret.insert(0, 1, '.');
        ret.insert(0, buffer);
        if (number < 0)
            ret.insert(0, 1, '-');

        while (ret[ret.size()-1] == '0')
            ret.erase(ret.size()-1);
        if (ret[ret.size()-1] == '.')
            ret.erase(ret.size()-1);

        return ret;
    }

    void operator <<=(SerializationInfo& si, const Milliseconds& timespan)
    {
        si <<= toString(timespan.totalUSecs(), 3);
        si.setTypeName("milliseconds");
    }

    void operator <<=(SerializationInfo& si, const Seconds& timespan)
    {
        si <<= timespan.totalSeconds();
        si.setTypeName("seconds");
    }

    void operator <<=(SerializationInfo& si, const Minutes& timespan)
    {
        si <<= timespan.totalMinutes();
        si.setTypeName("minutes");
    }

    void operator <<=(SerializationInfo& si, const Hours& timespan)
    {
        si <<= timespan.totalHours();
        si.setTypeName("hours");
    }

    void operator <<=(SerializationInfo& si, const Days& timespan)
    {
        si <<= timespan.totalDays();
        si.setTypeName("days");
    }

}
