/*
 * Copyright (C) 2007 Tommi Maekitalo
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

#ifndef CXXTOOLS_PROPERTIES_H
#define CXXTOOLS_PROPERTIES_H

#include <string>
#include <map>
#include <cxxtools/char.h>
#include <cxxtools/textstream.h>

namespace cxxtools
{
  class Properties
  {
      typedef std::map<String, String> ValuesType;
      ValuesType values;

    public:
      explicit Properties(const std::string& filename);
      explicit Properties(std::istream& in);

      void setValue(const String& key, const String& value)
      {
        values[key] = value;
      }

      void removeValue(const String& key)
      {
        values.erase(key);
      }

      bool hasValue(const String& key) const
      {
        return values.find(key) != values.end();
      }

      String getValue(const String& key, const String& def = String()) const
      {
        ValuesType::const_iterator it = values.find(key);
        return it == values.end() ? def : it->second;
      }

      template <typename OutputIterator>
      void getKeys(OutputIterator oi) const
      {
        for (ValuesType::const_iterator it = values.begin(); it != values.end(); ++it)
        {
          *oi = it->first;
          ++oi;
        }
      }

      template <typename OutputIterator>
      void getKeys(const String& praefix, OutputIterator oi) const
      {
        for (ValuesType::const_iterator it = values.begin(); it != values.end(); ++it)
        {
          String key = it->first;
          if (key.size() > praefix.size()
            && key.at(praefix.size()) == L'.'
            && key.compare(0, praefix.size(), praefix) == 0
            && key.find(L'.', praefix.size() + 2) == String::npos)
          {
            *oi = key.substr(praefix.size() + 1);
            ++oi;
          }
        }
      }

      template <typename OutputIterator>
      void getKeysLong(const String& praefix, OutputIterator oi) const
      {
        for (ValuesType::const_iterator it = values.begin(); it != values.end(); ++it)
        {
          String key = it->first;
          if (key.size() > praefix.size()
            && key.at(praefix.size()) == '.'
            && key.compare(0, praefix.size(), praefix) == 0
            && key.find('.', praefix.size() + 2) == String::npos)
          {
            *oi = key;
            ++oi;
          }
        }
      }

  };

  class PropertiesParser
  {
    public:
      class Event
      {
        public:
          // return true, if parser should stop
          virtual bool onKeyPart(const String& key) = 0;
          virtual bool onKey(const String& key) = 0;
          virtual bool onValue(const String& value) = 0;
      };

    private:
      Event& event;
      String key;
      String keypart;
      String value;
      Char::value_type unicode;
      unsigned unicodeCount;

      enum {
        state_0,
        state_key,
        state_key_esc,
        state_key_unicode,
        state_key_sp,
        state_value,
        state_value_esc,
        state_unicode,
        state_comment
      } state;

    public:
      PropertiesParser(Event& event_)
        : event(event_),
          state(state_0)
        { }

      void parse(TextIStream& in);
      void parse(std::istream& in, TextCodec<Char, char>* codec = 0);
      bool parse(Char ch);
      void end();
  };

}

#endif // CXXTOOLS_PROPERTIES_H
