/*
 * Copyright (C) 2015 Tommi Maekitalo
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

#ifndef CXXTOOLS_PROPERTIESSERIALIZER_H
#define CXXTOOLS_PROPERTIESSERIALIZER_H

#include <cxxtools/textstream.h>
#include <cxxtools/serializationinfo.h>

namespace cxxtools
{
    class PropertiesSerializer
    {
#if __cplusplus >= 201103L
            PropertiesSerializer(const PropertiesSerializer&) = delete;
            PropertiesSerializer& operator=(const PropertiesSerializer&) = delete;
#else
            PropertiesSerializer(const PropertiesSerializer&) { }
            PropertiesSerializer& operator=(const PropertiesSerializer&) { return *this; }
#endif

        public:
            PropertiesSerializer()
                : _outputSize(true),
                  _myts(0),
                  _ts(0)
            {
            }

            explicit PropertiesSerializer(std::basic_ostream<cxxtools::Char>& ts)
                : _outputSize(true),
                  _myts(0),
                  _ts(0)
            {
                begin(ts);
            }

            explicit PropertiesSerializer(std::ostream& os,
                TextCodec<cxxtools::Char, char>* codec = 0)
                : _outputSize(true),
                  _myts(0),
                  _ts(0)
            {
                begin(os, codec);
            }

            ~PropertiesSerializer()
            {
                delete _myts;
            }

            PropertiesSerializer& begin(std::basic_ostream<cxxtools::Char>& ts);

            PropertiesSerializer& begin(std::ostream& os,
                TextCodec<cxxtools::Char, char>* codec = 0);

            void finish();

            template <typename T>
            PropertiesSerializer& serialize(const T& v, const std::string& name)
            {
                SerializationInfo si;
                si <<= v;
                format(si, name);
                return *this;
            }

            template <typename T>
            PropertiesSerializer& serialize(const T& v)
            {
                SerializationInfo si;
                si <<= v;
                format(si, si.name());
                return *this;
            }

            bool outputSize() const     { return _outputSize; }

            void outputSize(bool sw)    { _outputSize = sw; }


        private:
            void format(const SerializationInfo& si, const std::string& key);
            void doFormat(const SerializationInfo& si, const std::string& key);

            bool _outputSize;
            std::basic_ostream<cxxtools::Char>* _myts;
            std::basic_ostream<cxxtools::Char>* _ts;
    };

}

#endif // CXXTOOLS_PROPERTIESSERIALIZER_H

