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
