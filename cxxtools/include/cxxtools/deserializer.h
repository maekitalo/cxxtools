/*
 * Copyright (C) 2011 Tommi Maekitalo
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

#ifndef CXXTOOLS_DESERIALIZER_H
#define CXXTOOLS_DESERIALIZER_H

#include <cxxtools/composer.h>

namespace cxxtools
{
    class CXXTOOLS_API Deserializer
    {
            // make non copyable
            Deserializer(const Deserializer&)  { }
            Deserializer& operator= (const Deserializer&) { }

        public:
            Deserializer()
            { }

            virtual ~Deserializer()
            { }

            /** @brief Deserialize an object

                This method will deserialize the object \a type from an
                input format. The type \a type must be serializable.
            */
            template <typename T>
            void deserialize(T& type)
            {
                Composer<T> composer;
                composer.begin(type);
                get(&composer);
                composer.fixup();
            }

            template <typename T>
            void deserialize(T& type, const std::string& name)
            {
                Composer<T> composer;
                composer.begin(type);
                get(&composer);
                composer.fixupMember(name);
            }

        protected:
            virtual void get(IComposer* composer) = 0;
    };
}

#endif // CXXTOOLS_DESERIALIZER_H
