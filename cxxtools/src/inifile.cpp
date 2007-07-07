/*
 * cxxtools - general purpose C++-toolbox
 * Copyright (C) 2007 Tommi Maekitalo
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

#include <cxxtools/inifile.h>
#include <cxxtools/iniparser.h>
#include <cxxtools/log.h>
#include <fstream>
#include <stdexcept>

log_define("cxxtools.inifile")

namespace cxxtools
{
  namespace
  {
    class IniFileEvent : public IniParser::Event
    {
        IniFile& iniFile;

        std::string section;
        std::string key;

      public:
        IniFileEvent(IniFile& iniFile_)
          : iniFile(iniFile_)
          { }

        bool onSection(const std::string& section);
        bool onKey(const std::string& key);
        bool onValue(const std::string& value);
    };

      bool IniFileEvent::onSection(const std::string& section_)
      {
        log_debug("section \"" << section_ << '"');
        section = section_;
        return false;
      }

      bool IniFileEvent::onKey(const std::string& key_)
      {
        log_debug("key \"" << key_ << '"');
        key = key_;
        return false;
      }

      bool IniFileEvent::onValue(const std::string& value)
      {
        log_debug("value(" << section << ", " << key << ")=" << value);
        iniFile.setValue(section, key, value);
        return false;
      }

  }

  IniFile::IniFile(const std::string& filename)
  {
    log_debug("read ini-file \"" << filename << '"');
    std::ifstream in(filename.c_str());
    if (!in)
      throw std::runtime_error("could not open file \"" + filename + '"');
    IniFileEvent ev(*this);
    IniParser(ev).parse(in);
  }

  IniFile::IniFile(std::istream& in)
  {
    IniFileEvent ev(*this);
    IniParser(ev).parse(in);
  }

  std::ostream& operator << (std::ostream& out, const IniFile& ini)
  {
    for (IniFile::MapType::const_iterator si = ini.data.begin();
         si != ini.data.end(); ++si)
    {
      out << '[' << si->first << "]\n";
      for (IniFile::MapType::mapped_type::const_iterator it = si->second.begin();
           it != si->second.end(); ++it)
        out << it->first << '=' << it->second << '\n';
    }
    return out;
  }
}
