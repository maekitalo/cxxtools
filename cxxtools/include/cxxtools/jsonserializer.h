/*
 * Copyright (C) 2009 Tommi Maekitalo, Marc Boris Duerner
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

#ifndef CXXTOOLS_JSONSERIALIZER_H
#define CXXTOOLS_JSONSERIALIZER_H

#include <cxxtools/formatter.h>
#include <cxxtools/textstream.h>
#include <cxxtools/serializer.h>
#include <cxxtools/noncopyable.h>

namespace cxxtools
{
    class JsonFormatter : public Formatter
    {
        public:
            JsonFormatter()
                : _ts(0),
                  _level(1),
                  _lastLevel(0),
                  _beautify(false)
            {
            }

            explicit JsonFormatter(std::basic_ostream<cxxtools::Char>& ts)
                : _ts(0),
                  _level(1),
                  _lastLevel(0),
                  _beautify(false)
            {
                begin(ts);
            }

            void begin(std::basic_ostream<cxxtools::Char>& ts);

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

            bool beautify() const     { return _beautify; }

            void beautify(bool sw)    { _beautify = sw; }

        private:
            void indent();
            void stringOut(const std::string& str);
            void stringOut(const cxxtools::String& str);

            std::basic_ostream<cxxtools::Char>* _ts;
            unsigned _level;
            unsigned _lastLevel;
            bool _beautify;
    };

    class JsonSerializer : private NonCopyable
    {
        public:
            JsonSerializer()
                : _ts(0)
            {
            }

            explicit JsonSerializer(std::basic_ostream<cxxtools::Char>& ts)
                : _ts(0)
            {
                _formatter.begin(ts);
            }

            explicit JsonSerializer(std::ostream& os,
                TextCodec<cxxtools::Char, char>* codec = 0);

            ~JsonSerializer()
            {
                delete _ts;
            }

            JsonSerializer& begin(std::basic_ostream<cxxtools::Char>& ts)
            {
                _formatter.begin(ts);
                return *this;
            }

            JsonSerializer& begin(std::ostream& os,
                TextCodec<cxxtools::Char, char>* codec = 0);

            void finish()
            {
                _formatter.finish();
            }

            template <typename T>
            JsonSerializer& serialize(const T& v, const std::string& name)
            {
                cxxtools::Serializer<T> s;
                s.begin(v);
                s.setName(name);
                s.format(_formatter);
                return *this;
            }

            bool beautify() const     { return _formatter.beautify(); }

            void beautify(bool sw)    { _formatter.beautify(sw); }


        private:
            JsonFormatter _formatter;
            std::basic_ostream<cxxtools::Char>* _ts;
    };
}

#endif // CXXTOOLS_JSONSERIALIZER_H

