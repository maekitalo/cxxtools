/* cxxtools/iniclass.h
   Copyright (C) 2003 Tommi Mäkitalo

This file is part of cxxtools.

Cxxtools is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Cxxtools is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with cxxtools; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330,
Boston, MA  02111-1307  USA
*/

#ifndef INILIB_H
#define INILIB_H

#include <iostream>
#include <strstream>
#include <sstream>
#include <map>

class IniFile;


template <class T>
T getValue(const IniFile& inifile, const std::string& section,
           const std::string& token, const T& def);

class IniFile : public std::map<std::string, std::map<std::string, std::string> >
{
  public:
    typedef std::string   section_name_type;
    typedef mapped_type   section_type;
    typedef std::string   token_name_type;
    typedef std::string   token_type;
    typedef section_type::iterator       section_iterator;
    typedef section_type::const_iterator const_section_iterator;

    IniFile()
    { }

    IniFile(const char* filename)
    { Read(filename); }

    IniFile(std::istream& in)
    { Read(in); }

    void Read(std::istream& in);
    void Read(const char* filename);

    bool Exists(const std::string& section) const
    { return find(section) != end(); }

    bool Exists(const std::string& section,
                const std::string& token) const
    {
      const_iterator section_it = find(section);
      return    section_it != end()
             && (*section_it).second.find(token) != (*section_it).second.end();
    }

    std::string getStringValue(const std::string& section,
                               const std::string& token,
                               const std::string& def) const;
    int getIntValue(const std::string& section,
                    const std::string& token,
                     int def ) const
    {
      return getValue(*this, section, token, def);
    }

#if USE_MEMBER_TEMPLATES
    template <class T>
     const T& getValue(const std::string& section,
                       const std::string& token,
                       const T& def) const
    {
      // find section

      const_iterator section_it = find(section);

      if (section_it != end())
      {
        // find token

        const_section_iterator token_it = (*section_it).second.find(token);

        if (token_it != (*section_it).end())
        {
          // extract value with stream
          T value;
          std::istrstream s((*token_it).second);
          s >> value;

          if (s)
            return value;
        }
      }

      return def;
    }
#endif
};

template <class T>
T getValue(const IniFile& inifile, const std::string& section,
           const std::string& token, const T& def)
{
  // find section

  IniFile::const_iterator section_it = inifile.find(section);

  if (section_it != inifile.end())
  {
    // find token

    IniFile::const_section_iterator token_it = (*section_it).second.find(token);

    if (token_it != (*section_it).second.end())
    {
      // extract value with stream
      T value;

      if (std::istringstream((*token_it).second) >> value)
        return value;
    }
  }

  return def;
}

std::ostream& operator << (std::ostream& out, const IniFile& ini);

#endif // INILIB_H
