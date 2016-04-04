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
#include <sys/time.h>
#include <time.h>
#include <iostream>

namespace cxxtools
{
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

        void get(const SerializationInfo& si, Timespan& timespan, uint64_t res)
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

    void operator <<=(SerializationInfo& si, const Milliseconds& timespan)
    {
        si <<= timespan.totalMSecs();
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
