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

#ifndef CXXTOOLS_JSONFORMATTER_H
#define CXXTOOLS_JSONFORMATTER_H

#include <cxxtools/formatter.h>
#include <iosfwd>

namespace cxxtools
{
    class JsonFormatter : public Formatter
    {
        public:
            JsonFormatter()
                : _os(0),
                  _level(1),
                  _lastLevel(0),
                  _beautify(false),
                  _plainkey(false),
                  _inputUtf8(false)
            {
            }

            explicit JsonFormatter(std::ostream& out)
                : _os(0),
                  _level(1),
                  _lastLevel(0),
                  _beautify(false),
                  _plainkey(false),
                  _inputUtf8(false)
            {
                begin(out);
            }

            void begin(std::ostream& out);

            void finish();

            virtual void addValueString(const std::string& name, const std::string& type,
                                  const String& value);

            virtual void addValueStdString(const std::string& name, const std::string& type,
                                  const std::string& value);

            virtual void addValueBool(const std::string& name, const std::string& type,
                                  bool value);

            virtual void addValueInt(const std::string& name, const std::string& type,
                                  int_type value);

            virtual void addValueUnsigned(const std::string& name, const std::string& type,
                                  unsigned_type value);

            virtual void addValueFloat(const std::string& name, const std::string& type,
                                  float value);

            virtual void addValueDouble(const std::string& name, const std::string& type,
                                  double value);

            virtual void addValueLongDouble(const std::string& name, const std::string& type,
                                  long double value);

            virtual void addNull(const std::string& name, const std::string& type);

            virtual void beginArray(const std::string& name, const std::string& type);

            virtual void finishArray();

            virtual void beginObject(const std::string& name, const std::string& type);

            virtual void beginMember(const std::string& name);

            virtual void finishMember();

            virtual void finishObject();

            bool beautify() const     { return _beautify; }

            void beautify(bool sw)    { _beautify = sw; }

            bool plainkey() const     { return _plainkey; }

            void plainkey(bool sw)    { _plainkey = sw; }

            bool inputUtf8() const    { return _inputUtf8; }

            void inputUtf8(bool sw)   { _inputUtf8 = sw; }

            void beginValue(const std::string& name);

            void finishValue();

        private:
            void indent();
            void stringOut(const std::string& str);
            void stringOut(const cxxtools::String& str);

            std::ostream* _os;
            unsigned _level;
            unsigned _lastLevel;
            bool _beautify;
            bool _plainkey;
            bool _inputUtf8;
    };

}

#endif // CXXTOOLS_JSONFORMATTER_H

