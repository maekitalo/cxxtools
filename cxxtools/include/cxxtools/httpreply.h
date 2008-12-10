/*
 * Copyright (C) 2005 Tommi Maekitalo
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

#ifndef CXXTOOLS_HTTPREPLY_H
#define CXXTOOLS_HTTPREPLY_H

#include <iostream>
#include <map>

namespace cxxtools
{
  class HttpRequest;

  /**
   Reads a http-reply from a input stream and parses it.

   cxxtools::HttpReply helps parsing a http reply sent from a http-server.
   It is itself a istream, which returns the body from the http-reply.

   See cxxtools::HttpRequest for a simple use case.

   */
  class HttpReply : public std::istream
  {
      class Parser;
      friend class Parser;

      void parse_header();

      typedef std::map<std::string, std::string> header_type;
      header_type header;
      unsigned returncode;

    public:
      /// Instantiates an empty http reply object.
      /// To associate a request with this object, you may use std::ios::rdbuf.
      HttpReply()
        : std::istream(0),
          returncode(0)
        { }

      /// Executes the request and starts reading the reply from the server.
      HttpReply(HttpRequest& request);

      /// Reads a http-reply from a input stream.
      explicit HttpReply(std::istream& request)
        : std::istream(request.rdbuf())
      {
        parse_header();
      }

      /// Reads a reply from a request object.
      /// The the request is executed, if not done already.
      void get(HttpRequest& request);

      /// Returns a http-reply header.
      /// If the header is not set, the passed default value is returned
      const std::string& getHeader(const std::string& name, const std::string& def = std::string()) const
      {
        header_type::const_iterator it = header.find(name);
        return it == header.end() ? def : it->second;
      }

      /// Returns all http-reply-headers to the output-iterator.
      /// Only the key of the headers are returned, so the dereferenced output
      /// iterator need to have an assignment for a std::string.
      template <typename outputIterator>
      void getHeaders(outputIterator oit) const
      {
        for (header_type::const_iterator it = header.begin(); it != header.end(); ++it)
          (*oit++) = it->first;
      }

      /// Returns the http-return code, received from the server.
      unsigned getReturnCode() const  { return returncode; }
  };
}

#endif // CXXTOOLS_HTTPREPLY_H

