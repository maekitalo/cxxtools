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

#ifndef CXXTOOLS_INIFILE_H
#define CXXTOOLS_INIFILE_H

#include <iostream>
#include <string>
#include <sstream>
#include <map>

namespace cxxtools
{
  class IniFile
  {
      friend std::ostream& operator << (std::ostream& out, const IniFile& ini);

      typedef std::map<std::string, std::map<std::string, std::string> > MapType;
      MapType data;

    public:
      IniFile()  { }
      explicit IniFile(const std::string& filename);
      explicit IniFile(std::istream& in);

      /// Returns true, if section exists.
      bool exists(const std::string& section) const
      { return data.find(section) != data.end(); }

      /// Returns true, if key exists in section exists.
      bool exists(const std::string& section,
                  const std::string& token) const
      {
        MapType::const_iterator si = data.find(section);
        return    si != data.end()
               && si->second.find(token) != si->second.end();
      }

      /// Returns associated value from section-key-pair or default-value.
      std::string getValue(const std::string& section,
         const std::string& token, const std::string& def = std::string()) const
      {
        // find section

        MapType::const_iterator si = data.find(section);

        if (si != data.end())
        {
          // find token

          MapType::mapped_type::const_iterator token_it = si->second.find(token);

          if (token_it != si->second.end())
            return token_it->second;
        }

        return def;
      }

      /// Get the value and convert it with istream-operator.
      /// The return-type is identified by the default-value-type.
      template <typename T>
       T getValueT(const std::string& section,
                  const std::string& token,
                  const T& def) const
      {
        // find section

        MapType::const_iterator si = data.find(section);

        if (si != data.end())
        {
          // find token

          MapType::mapped_type::const_iterator token_it = si->second.find(token);

          if (token_it != si->second.end())
          {
            // extract value with stream
            T value;
            std::istringstream s(token_it->second);
            s >> value;

            if (s)
              return value;
          }
        }

        return def;
      }

      /// setting a new value
      void setValue(const std::string& section, const std::string& key,
        const std::string& value)
      {
        data[section][key] = value;
      }

      /// setting a new value with a type (need output-operator for ostream).
      template <typename T>
      void setValueT(const std::string& section, const std::string& key,
        const T& value)
      {
        std::ostringstream v;
        v << value;
        data[section][key] = v.str();
      }

      /**
       * Get the names of sections.
       * example1:
       * <code>
       *  cxxtools::IniFile ini("my.ini");
       *
       *  // copy names of sections into a container
       *  std::set<std::string> keys;
       *  ini.getSections(std::inserter(s, s.begin());
       *
       *  // or printing the names:
       *  ini.getSections(std::ostream_iterator(std::cout, "\n"));
       * </code>
       */
      template <typename OutputIterator>
      void getSections(OutputIterator oi)
      {
        for (MapType::const_iterator it = data.begin(); it != data.end(); ++it, ++oi)
          *oi = it->first;
      }

      /**
       * Get the keys of a section.
       * example1:
       * <code>
       *  cxxtools::IniFile ini("my.ini");
       *
       *  // copy keys into a container
       *  std::set<std::string> keys;
       *  ini.getKeys("section2", std::inserter(s, s.begin());
       *
       *  // or printing the keys:
       *  ini.getKeys("section2", std::ostream_iterator(std::cout, "\n"));
       * </code>
       */
      template <typename OutputIterator>
      void getKeys(const std::string& section, OutputIterator oi)
      {
        MapType::const_iterator si = data.find(section);
        if (si != data.end())
        {
          for (MapType::mapped_type::const_iterator it = si->second.begin();
               it != si->second.end(); ++it, ++oi)
            *oi = it->first;
        }
      }

  };

  /// Outputs ini-file to a output-stream
  std::ostream& operator << (std::ostream& out, const IniFile& ini);

}

#endif // CXXTOOLS_INIFILE_H
