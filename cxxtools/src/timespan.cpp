/*
 * Copyright (C) 2008 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
 */

#include <cxxtools/timespan.h>
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

    std::ostream& operator<< (std::ostream& out, const Timespan& ht)
    {
        out << static_cast<double>(ht.toUSecs()) / 1e6;
        return out;
    }
}
