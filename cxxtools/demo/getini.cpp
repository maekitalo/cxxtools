/* getini.cpp
 * read value from ini-file
 *
 * cxxtools - general purpose C++-toolbox
 * Copyright (C) 2003 Tommi Maekitalo
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

#include <exception>
#include <iostream>
#include <fstream>
#include <cxxtools/iniparser.h>
#include <cxxtools/loginit.h>

log_define("cxxtools.getini")

class GetIni : public cxxtools::IniParser::Event
{
    std::string section;
    std::string key;
    std::string value;
    bool inSection;
    bool inKey;

  public:
    GetIni(const std::string& section_, const std::string& key_,
        const std::string& defvalue_) 
      : section(section_),
        key(key_),
        value(defvalue_),
        inSection(false),
        inKey(false)
        { }

    bool onSection(const std::string& section);
    bool onKey(const std::string& key);
    bool onValue(const std::string& value);

    const std::string& getValue() const  { return value; }
};

bool GetIni::onSection(const std::string& section_)
{
  inSection = (section == section_);
  log_debug("onSection(" << section_ << ") => " << inSection);
  return false;
}

bool GetIni::onKey(const std::string& key_)
{
  inKey = (inSection && key == key_);
  log_debug("onKey(" << key_ << ") => " << inKey);
  return false;
}

bool GetIni::onValue(const std::string& value_)
{
  log_debug("onValue(" << value_ << ") => " << inKey);
  if (inKey)
  {
    log_debug("value found");
    value = value_;
    return true;
  }
  return false;
}

int main(int argc, char* argv[])
{
  log_init();

  try
  {
    if (argc < 4)
    {
      std::cerr << "usage: " << argv[0]
        << " inifile section key [default]" << std::endl;
      return -1;
    }

    const char* fname = argv[1];
    const char* section = argv[2];
    const char* key = argv[3];
    const char* defvalue = argv[4] ? argv[4] : "";

    std::ifstream in(fname);
    GetIni ini(section, key, defvalue);
    cxxtools::IniParser(ini).parse(in);
    std::cout << ini.getValue() << std::endl;
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

