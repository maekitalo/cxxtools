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
#include "cxxtools/timespan.h"
#include <algorithm>

namespace cxxtools {

const cxxtools::int64_t Timespan::Milliseconds = 1000;
const cxxtools::int64_t Timespan::Seconds      = 1000 * Timespan::Milliseconds;
const cxxtools::int64_t Timespan::Minutes      =   60 * Timespan::Seconds;
const cxxtools::int64_t Timespan::Hours        =   60 * Timespan::Minutes;
const cxxtools::int64_t Timespan::Days         =   24 * Timespan::Hours;


Timespan::Timespan():
	_span(0)
{
}


Timespan::Timespan(cxxtools::int64_t microseconds)
: _span(microseconds)
{
}


Timespan::Timespan(long seconds, long microseconds)
: _span(cxxtools::int64_t(seconds)*Seconds + microseconds)
{
}


Timespan::Timespan(int days, int hours, int minutes, int seconds, int microseconds)
: _span( cxxtools::int64_t(microseconds) +
         cxxtools::int64_t(seconds)*Seconds +
         cxxtools::int64_t(minutes)*Minutes +
         cxxtools::int64_t(hours)*Hours +
         cxxtools::int64_t(days)*Days )
{
}


Timespan::Timespan(const Timespan& timespan):
	_span(timespan._span)
{
}


Timespan::~Timespan()
{
}


Timespan& Timespan::operator = (const Timespan& timespan)
{
	_span = timespan._span;
	return *this;
}


Timespan& Timespan::operator = (cxxtools::int64_t microseconds)
{
	_span = microseconds;
	return *this;
}


Timespan& Timespan::set(int days, int hours, int minutes, int seconds, int microseconds)
{
	_span = cxxtools::int64_t(microseconds) +
            cxxtools::int64_t(seconds)*Seconds +
            cxxtools::int64_t(minutes)*Minutes +
            cxxtools::int64_t(hours)*Hours +
            cxxtools::int64_t(days)*Days;
	return *this;
}


Timespan& Timespan::set(long seconds, long microseconds)
{
	_span = cxxtools::int64_t(seconds)*Seconds + cxxtools::int64_t(microseconds);
	return *this;
}


void Timespan::swap(Timespan& timespan)
{
	std::swap(_span, timespan._span);
}


Timespan Timespan::operator + (const Timespan& d) const
{
	return Timespan(_span + d._span);
}


Timespan Timespan::operator - (const Timespan& d) const
{
	return Timespan(_span - d._span);
}


Timespan& Timespan::operator += (const Timespan& d)
{
	_span += d._span;
	return *this;
}


Timespan& Timespan::operator -= (const Timespan& d)
{
	_span -= d._span;
	return *this;
}


Timespan Timespan::operator + (cxxtools::int64_t microseconds) const
{
	return Timespan(_span + microseconds);
}


Timespan Timespan::operator - (cxxtools::int64_t microseconds) const
{
	return Timespan(_span - microseconds);
}


Timespan& Timespan::operator += (cxxtools::int64_t microseconds)
{
	_span += microseconds;
	return *this;
}


Timespan& Timespan::operator -= (cxxtools::int64_t microseconds)
{
	_span -= microseconds;
	return *this;
}


} // namespace cxxtools
