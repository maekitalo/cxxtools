/*
 * Copyright (C) 2004-2007 Marc Boris Duerner
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

#include <cxxtools/facets.h>

locale::id numpunct<cxxtools::Char>::id;


numpunct<cxxtools::Char>::numpunct(size_t refs)
: locale::facet(refs)
{ }


numpunct<cxxtools::Char>::~numpunct()
{ }


cxxtools::Char numpunct<cxxtools::Char>::decimal_point() const
{ return this->do_decimal_point(); }


cxxtools::Char numpunct<cxxtools::Char>::thousands_sep() const
{ return this->do_thousands_sep(); }


string numpunct<cxxtools::Char>::grouping() const
{ return this->do_grouping(); }


cxxtools::String numpunct<cxxtools::Char>::truename() const
{ return this->do_truename(); }


cxxtools::String numpunct<cxxtools::Char>::falsename() const
{ return this->do_falsename(); }


cxxtools::Char numpunct<cxxtools::Char>::do_decimal_point() const
{ return '.'; }


cxxtools::Char numpunct<cxxtools::Char>::do_thousands_sep() const
{ return ','; }


std::string numpunct<cxxtools::Char>::do_grouping() const
{ return ""; }


cxxtools::String numpunct<cxxtools::Char>::do_truename() const
{
    static const cxxtools::Char truename[] = {'t', 'r', 'u', 'e', '\0'};
    return truename;
}


cxxtools::String numpunct<cxxtools::Char>::do_falsename() const
{
    static const cxxtools::Char falsename[] = {'f', 'a', 'l', 's', 'e', '\0'};
    return falsename;
}

}
