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

#ifndef CXXTOOLS_DESERIALIZER_H
#define CXXTOOLS_DESERIALIZER_H

#include <cxxtools/serializationerror.h>
#include <cxxtools/composer.h>
#include <cxxtools/config.h>
#include <stack>

namespace cxxtools
{
    /**
     * convert format to SerializationInfo
     */
    class Deserializer
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

            Deserializer()
            { }

            virtual ~Deserializer()
            { }

            /** @brief Deserialize an object

                This method will deserialize the object \a type from an
                input format. The type \a type must be serializable.
            */
            template <typename T>
            void deserialize(T& type) const
            {
                if (current() == 0)
                    throw SerializationError("no data was processed");

                *current() >>= type;
            }

            template <typename T>
            void deserialize(T& type, const std::string& name) const
            {
                const SerializationInfo* p;

                if (current() == 0)
                    throw SerializationError("no data was processed");

                if ((p = current()->findMember(name)) == 0)
                    throw SerializationMemberNotFound(*current(), name);

                *p >>= type;
            }

            SerializationInfo& si()
            { return _si; }

            const SerializationInfo& si() const
            { return _si; }

            void begin();

            void clear();

            SerializationInfo* current()
            { return _current.empty() ? 0 : _current.top(); }

            const SerializationInfo* current() const
            { return _current.empty() ? 0 : _current.top(); }

            void setCategory(SerializationInfo::Category category)
            { current()->setCategory(category); }

            void setName(const std::string& name)
            { current()->setName(name); }

            void setName(std::string&& name)
            { current()->setName(std::move(name)); }

            void setTypeName(const std::string& type)
            { current()->setTypeName(type); }

            void setTypeName(std::string&& type)
            { current()->setTypeName(std::move(type)); }

            void setValue(String&& value)
            { current()->setValue(std::move(value)); }

            void setValue(std::string&& value)
            { current()->setValue(std::move(value)); }

            void setValue(const char* value)
            { current()->setValue(value); }

            void setValue(bool value)
            { current()->setValue(value); }

            void setValue(char value)
            { current()->setValue(value); }

            void setValue(int_type value)
            { current()->setValue(value); }

            void setValue(unsigned_type value)
            { current()->setValue(value); }

            void setValue(long double value)
            { current()->setValue(value); }

            void setNull()
            { current()->setNull(); }

            void beginMember(const std::string& name, const std::string& type, SerializationInfo::Category category);

            void leaveMember();

        private:
            SerializationInfo _si;
            std::stack<SerializationInfo*> _current;
    };

}

#endif // CXXTOOLS_DESERIALIZER_H
