/* cxxtools/httpreply.h
 *
 * cxxtools - general purpose C++-toolbox
 * Copyright (C) 2005 Tommi Maekitalo
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

#ifndef CXXTOOLS_HTTPREPLY_H
#define CXXTOOLS_HTTPREPLY_H

#include <iostream>
#include <map>

namespace cxxtools
{
  class HttpRequest;

  class HttpReply : public std::istream
  {
      class Parser;
      friend class Parser;

      void parse_header();

      typedef std::map<std::string, std::string> header_type;
      header_type header;
      unsigned returncode;

    public:
      HttpReply(HttpRequest& request);

      HttpReply(std::istream& request)
        : std::istream(request.rdbuf())
      {
        parse_header();
      }

      const std::string& getHeader(const std::string& name, const std::string& def) const
      {
        header_type::const_iterator it = header.find(name);
        return it == header.end() ? def : it->second;
      }

      unsigned getReturnCode()  { return returncode; }
  };
}

#endif // CXXTOOLS_HTTPREPLY_H

