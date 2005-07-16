/* cxxtools/iniclass.h
   Copyright (C) 2003 Tommi Maekitalo

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

#ifndef CXXTOOLS_INICLASS_H
#define CXXTOOLS_INICLASS_H

#include <iostream>
#include <sstream>
#include <map>

namespace cxxtools
{

/**
 read/write configurationfile in ini-format.

 Ini-files are files, which are seperated into sections. Each section
 has a title, bracketed with '[' and ']'. Each Section contains lines with
 name-value-pairs devided with '='.

 This class is a map of sections, each of them is a map of strings.

 Lines, which does not contain a section-title nor a '=' are ignored.

 example of a ini-file:
 \code
 [section1]
 key1 = value
 key2 = other value
 [other section]
 # a comment
 key = value
 this is also a comment, because it contains no equals-symbol nor a section-title
 \endcode
 */
class ini_file : public std::map<std::string, std::map<std::string, std::string> >
{
  public:
    typedef std::string   section_name_type;
    typedef mapped_type   section_type;
    typedef std::string   token_name_type;
    typedef std::string   token_type;
    typedef section_type::iterator       section_iterator;
    typedef section_type::const_iterator const_section_iterator;

    /// default constructor
    ini_file()
    { }

    /// read ini-file from file
    ini_file(const char* filename)
    { read(filename); }

    /// read ini-file from inputstream
    ini_file(std::istream& in)
    { read(in); }

    /// read ini-file from inputstream
    void read(std::istream& in);

    /// read ini-file from file
    void read(const char* filename);

    /// returns true, if section exists
    bool exists(const std::string& section) const
    { return find(section) != end(); }

    /// returns true, if key exists in section exists
    bool exists(const std::string& section,
                const std::string& token) const
    {
      const_iterator section_it = find(section);
      return    section_it != end()
             && (*section_it).second.find(token) != (*section_it).second.end();
    }

    /// returns associated value from section-key-pair or default-value.
    /// The value is converted to the return-type with operator>>(istream&, T&).
    /// The return-type is identified by the default-value-type.
    template <typename T>
     T get_value(const std::string& section,
                const std::string& token,
                const T& def) const
    {
      // find section

      const_iterator section_it = find(section);

      if (section_it != end())
      {
        // find token

        const_section_iterator token_it = section_it->second.find(token);

        if (token_it != section_it->second.end())
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

    /// returns associated value from section-key-pair or default-value
    const std::string& get_value(const std::string& section,
       const std::string& token, const std::string& def = std::string()) const
    {
      // find section

      const_iterator section_it = find(section);

      if (section_it != end())
      {
        // find token

        const_section_iterator token_it = section_it->second.find(token);

        if (token_it != section_it->second.end())
          return token_it->second;
      }

      return def;
    }

    /// returns associated value from section-key-pair or default-value
    std::string get_value(const std::string& section,
       const std::string& token, const char* def) const
    {
      // find section

      const_iterator section_it = find(section);

      if (section_it != end())
      {
        // find token

        const_section_iterator token_it = section_it->second.find(token);

        if (token_it != section_it->second.end())
          return token_it->second;
      }

      return def;
    }

};

/// Outputs ini-file to a output-stream
std::ostream& operator << (std::ostream& out, const ini_file& ini);

}

#endif // CXXTOOLS_INICLASS_H
