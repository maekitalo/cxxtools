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

#ifndef CXXTOOLS_BINSERIALIZER_H
#define CXXTOOLS_BINSERIALIZER_H

#include <cxxtools/formatter.h>
#include <cxxtools/serializer.h>
#include <cxxtools/textstream.h>
#include <cxxtools/noncopyable.h>

namespace cxxtools
{
    class BinFormatter : public Formatter
    {
        public:
            BinFormatter();

            explicit BinFormatter(std::ostream& out);

            void begin(std::ostream& out);

            void finish();

            virtual void addValue(const std::string& name, const std::string& type,
                                  const cxxtools::String& value, const std::string& id);

            virtual void addReference(const std::string& name, const cxxtools::String& value);

            virtual void beginArray(const std::string& name, const std::string& type,
                                    const std::string& id);

            virtual void finishArray();

            virtual void beginObject(const std::string& name, const std::string& type,
                                     const std::string& id);

            virtual void beginMember(const std::string& name);

            virtual void finishMember();

            virtual void finishObject();

        private:
            std::ostream* _out;
            TextOStream _ts;
    };

    class BinSerializer : private NonCopyable
    {
        public:
            BinSerializer()
            {
            }

            explicit BinSerializer(std::ostream& out)
            {
                _formatter.begin(out);
            }

            BinSerializer& begin(std::ostream& out)
            {
                _formatter.begin(out);
                return *this;
            }

            template <typename T>
            BinSerializer& serialize(const T& v, const std::string& name)
            {
                cxxtools::Serializer<T> s;
                s.begin(v);
                s.setName(name);
                s.format(_formatter);
                return *this;
            }

            void finish()
            { }

        private:
            BinFormatter _formatter;
    };
}

#endif // CXXTOOLS_BINSERIALIZER_H

