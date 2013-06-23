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

#include <cxxtools/propertiesfile.h>
#include <cxxtools/propertiesparser.h>
#include <fstream>

namespace cxxtools
{
  namespace
  {
    class PropertiesEvent : public PropertiesParser::Event
    {
        PropertiesFile& properties;
        String key;

      public:
        PropertiesEvent(PropertiesFile& properties_)
          : properties(properties_)
          { }
        bool onKeyPart(const String& key);
        bool onKey(const String& key);
        bool onValue(const String& value);
    };

    bool PropertiesEvent::onKeyPart(const String&)
    {
      return false;
    }

    bool PropertiesEvent::onKey(const String& key_)
    {
      key = key_;
      return false;
    }

    bool PropertiesEvent::onValue(const String& value)
    {
      properties.setValue(key, value);
      return false;
    }

  }

  PropertiesFile::PropertiesFile(const std::string& filename)
  {
    PropertiesEvent ev(*this);
    std::ifstream in(filename.c_str());
    if (!in)
      throw PropertiesParserError("could not open properties file \"" + filename + '"');
    PropertiesParser(ev).parse(in);
  }

  PropertiesFile::PropertiesFile(std::istream& in)
  {
    PropertiesEvent ev(*this);
    PropertiesParser(ev).parse(in);
  }

}
