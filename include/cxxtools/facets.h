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
#ifndef CXXTOOLS_FACETS_H
#define CXXTOOLS_FACETS_H

#include <cxxtools/string.h>
#include <iosfwd>

namespace std {

/** @brief Numpunct localization facet
    @ingroup Unicode
*/
template <>
class numpunct<cxxtools::Char> : public locale::facet {
    public:
        typedef cxxtools::Char char_type;
        typedef basic_string<cxxtools::Char> string_type;

        // gcc 3.4.x violates the c++ standard by requiring a __numpunct_cache
        #if __GLIBCXX__ <= 20051201 && __GLIBCXX__ >= 20040419
        typedef __numpunct_cache<cxxtools::Char>  __cache_type;
        #endif

        static locale::id id;
        virtual locale::id& __get_id (void) const { return id; }

    public:
        explicit numpunct(size_t refs = 0);

        virtual ~numpunct();

        char_type decimal_point() const;

        char_type thousands_sep() const;

        string grouping() const;

        string_type truename() const;

        string_type falsename() const;

    protected:
        virtual char_type do_decimal_point() const;

        virtual char_type do_thousands_sep() const;

        virtual string do_grouping() const;

        virtual string_type do_truename() const;

        virtual string_type do_falsename() const;
};

} // namespace std

namespace cxxtools {

static std::ios_base::Init cxxtools_stream_init;

static struct InitLocale
{
    InitLocale()
    {
        std::locale::global( std::locale(std::locale(), new std::ctype<cxxtools::Char>) );
        std::locale::global( std::locale(std::locale(), new std::numpunct<cxxtools::Char>) );
        std::locale::global( std::locale(std::locale(), new std::num_get<cxxtools::Char>) );
        std::locale::global( std::locale(std::locale(), new std::num_put<cxxtools::Char>) );
    }
} cxxtools_init_locale;

}

#endif
