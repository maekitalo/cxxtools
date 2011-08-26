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

#include <cxxtools/deserializer.h>
#include <cxxtools/noncopyable.h>

namespace cxxtools
{
    class CXXTOOLS_API BinDeserializer : private NonCopyable
    {
        public:
            BinDeserializer(std::istream& in);

            template <typename T>
            void deserialize(T& type)
            {
                Deserializer<T> deser;
                deser.begin(type);
                this->get(&deser);
                deser.fixup(_context);
            }

            void finish()
            {
                _context.fixup();
                _context.clear();
            }

        protected:
            void get(IDeserializer* deser);

        private:
            BinDeserializer(BinDeserializer& s);
            void read(std::string& str);
            void processValueData(IDeserializer* deser);
            void processObjectMembers(IDeserializer* deser);
            void processArrayData(IDeserializer* deser);
            void processReference(IDeserializer* deser);

            std::istream& _in;
            std::string _type;
            cxxtools::DeserializationContext _context;
    };
}
