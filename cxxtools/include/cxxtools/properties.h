/*
 * cxxtools - general purpose C++-toolbox
 * Copyright (C) 2007 Tommi Maekitalo
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef CXXTOOLS_PROPERTIES_H
#define CXXTOOLS_PROPERTIES_H

#include <string>
#include <iosfwd>
#include <map>

namespace cxxtools
{
  class Properties
  {
      typedef std::map<std::string, std::string> ValuesType;
      ValuesType values;

    public:
      explicit Properties(const std::string& filename);
      explicit Properties(std::istream& in);

      void setValue(const std::string& key, const std::string& value)
      {
        values[key] = value;
      }

      void removeValue(const std::string& key)
      {
        values.erase(key);
      }

      bool hasValue(const std::string& key) const
      {
        return values.find(key) != values.end();
      }

      std::string getValue(const std::string& key, const std::string& def = std::string()) const
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
      void getKeys(const std::string& praefix, OutputIterator oi) const
      {
        for (ValuesType::const_iterator it = values.begin(); it != values.end(); ++it)
        {
          std::string key = it->first;
          if (key.size() > praefix.size()
            && key.at(praefix.size()) == '.'
            && key.compare(0, praefix.size(), praefix) == 0
            && key.find('.', praefix.size() + 2) == std::string::npos)
          {
            *oi = key.substr(praefix.size() + 1);
            ++oi;
          }
        }
      }

      template <typename OutputIterator>
      void getKeysLong(const std::string& praefix, OutputIterator oi) const
      {
        for (ValuesType::const_iterator it = values.begin(); it != values.end(); ++it)
        {
          std::string key = it->first;
          if (key.size() > praefix.size()
            && key.at(praefix.size()) == '.'
            && key.compare(0, praefix.size(), praefix) == 0
            && key.find('.', praefix.size() + 2) == std::string::npos)
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
          virtual bool onKeyPart(const std::string& key) = 0;
          virtual bool onKey(const std::string& key) = 0;
          virtual bool onValue(const std::string& value) = 0;
      };

    private:
      Event& event;
      std::string key;
      std::string keypart;
      std::string value;
      enum {
        state_0,
        state_key,
        state_key_sp,
        state_value,
        state_value_esc,
        state_comment
      } state;

    public:
      PropertiesParser(Event& event_)
        : event(event_),
          state(state_0)
        { }

      void parse(std::istream& in);
      bool parse(char ch);
      void end();
  };

}

#endif // CXXTOOLS_PROPERTIES_H
