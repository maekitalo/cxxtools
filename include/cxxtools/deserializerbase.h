/*
 * Copyright (C) 2011 Tommi Maekitalo
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

#ifndef CXXTOOLS_DESERIALIZERBASE_H
#define CXXTOOLS_DESERIALIZERBASE_H

#include <cxxtools/serializationinfo.h>
#include <cxxtools/api.h>

namespace cxxtools
{
    /**
     * convert format to SerializationInfo
     */
    class CXXTOOLS_API DeserializerBase
    {
        public:
#ifdef HAVE_LONG_LONG
            typedef long long int_type;
#else
            typedef long int_type;
#endif
#ifdef HAVE_UNSIGNED_LONG_LONG
            typedef unsigned long long unsigned_type;
#else
            typedef unsigned long unsigned_type;
#endif

            DeserializerBase()
                : _current(0)
            { }

            virtual ~DeserializerBase()
            { }

            void begin()
            {
                _current = &_si;
                _si.clear();
            }

            void clear()
            {
                _current = 0;
                _si.clear();
            }

            SerializationInfo* si()
            { return &_si; }

            const SerializationInfo* si() const
            { return &_si; }

            SerializationInfo* current()
            { return _current; }

            void setCategory(SerializationInfo::Category category);

            void setName(const std::string& name);

            void setValue(const String& value);

            void setValue(const std::string& value);

            void setValue(const char* value);

            void setValue(bool value);

            void setValue(int_type value);

            void setValue(unsigned_type value);

            void setValue(long double value);

            void setNull();

            void setTypeName(const std::string& type);

            void beginMember(const std::string& name, const std::string& type, SerializationInfo::Category category);

            void leaveMember();

        private:
            SerializationInfo _si;
            SerializationInfo* _current;
    };

}

#endif // CXXTOOLS_DESERIALIZERBASE_H
