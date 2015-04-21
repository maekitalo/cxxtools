/*
 * Copyright (C) 2013 Tommi Maekitalo
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

#ifndef CXXTOOLS_PROPERTIESDESERIALIZER_H
#define CXXTOOLS_PROPERTIESDESERIALIZER_H

#include <cxxtools/deserializer.h>
#include <cxxtools/textstream.h>

namespace cxxtools
{
    class PropertiesDeserializer : public Deserializer
    {
        class Ev;
        friend class Ev;

        bool _envSubst;
        bool _trim;

    public:
        PropertiesDeserializer(std::istream& in, TextCodec<Char, char>* codec = 0);

        PropertiesDeserializer(std::basic_istream<Char>& in);

        PropertiesDeserializer();

        /// Sets the envSubst flag.
        /// When set flag is set, environment variables are substituted in the
        /// value using / `cxxtools::envSubst`.
        void envSubst(bool sw) { _envSubst = sw; }

        /// Returns the envSubst flag.
        bool envSubst() const
        { return _envSubst; }

        /// Sets the trim flag.
        /// When set flag is set, white space is removed from the beginning
        /// and end of the values.
        void trim(bool sw)
        { _trim = sw; }

        /// Returns the trim flag.
        bool trim() const
        { return _trim; }

    private:
        void doDeserialize(std::basic_istream<Char>& in);
    };
}

#endif // CXXTOOLS_PROPERTIESDESERIALIZER_H

