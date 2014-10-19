/*
 * Copyright (C) 2010 Tommi Maekitalo
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

#ifndef CXXTOOLS_NET_URI_H
#define CXXTOOLS_NET_URI_H

#include <string>

namespace cxxtools
{
namespace net
{
  /** The Uri class represents a uniform resource identifier.

      A Uri is used to identify a web resource.
      
      This class can parse a string to split a uri into his parts or build a
      string from parts of the uri.

      The syntax of a uri is:

      \verbatim
        <scheme name> : <hierarchical part> [ ? <query> ] [ # <fragment> ]
      \endverbatim

      A example is here:

      \verbatim
        foo://username:password@example.com:8042/over/there/index.dtb?type=animal&name=narwhal#nose
        \_/   \______/ \______/ \_________/ \__/ \__________________/ \______________________/ \__/
         |       |                   |       |          |                      |                |
         |     user     password  hostname  port       path                  query          fragment
         |
         schema/protocol
      \endverbatim

      Example use:

      \code

        cxxtools::Uri u("http://www.tntnet.org/index.html");
        std::cout << "host=" << u.host() << std::endl;   // prints www.tntnet.org

        u.path("cxxtools.html");
        std::cout << "new uri " << u.str() << std::endl;   // prints http://www.tntnet.org/cxxtools.html

      \endcode
   */

  class Uri
  {
      bool _ipv6;
      std::string _protocol;
      std::string _user;
      std::string _password;
      std::string _host;
      unsigned short int _port;
      std::string _path;
      std::string _query;
      std::string _fragment;

      void parse(const char* str);

    public:
      /// Creates a empty Uri.
      Uri()  { }

      /// Parses the string str and set the elements of the Uri.
      Uri(const std::string& str)
        : _ipv6(false),
          _port(0)
      { parse(str.c_str()); }

      Uri(const char* str)
        : _ipv6(false),
          _port(0)
      { parse(str); }

      /// Sets the protocol part of the Uri.
      void protocol(const std::string& protocol)
        { _protocol = protocol; }

      /// Returns the protocol part of the Uri.
      const std::string& protocol() const
        { return _protocol; }

      /// Sets the user part of the Uri.
      void user(const std::string& user)
        { _user = user; }

      /// Returns the user part of the Uri.
      const std::string& user() const
        { return _user; }

      /// Sets the password part of the Uri.
      void password(const std::string& password)
        { _password = password; }

      /// Returns the password part of the Uri.
      const std::string& password() const
        { return _password; }

      /// Sets the host part of the Uri.
      void host(const std::string& host)
        { _host = host; }

      /// Returns the host part of the Uri.
      const std::string& host() const
        { return _host; }

      /// Sets the port part of the Uri.
      void port(unsigned short int p)
        { _port = p; }

      /// Returns the port part of the Uri.
      unsigned short int port() const
        { return _port; }

      /// Sets the path part of the Uri.
      void path(const std::string& path)
        { _path = path; }

      /// Returns the path part of the Uri.
      const std::string& path() const
        { return _path; }

      /// Sets the query part of the Uri.
      void query(const std::string& query)
        { _query = query; }

      /// Returns the query part of the Uri.
      const std::string& query() const
        { return _query; }

      /// Sets the fragment part of the Uri.
      void fragment(const std::string& fragment)
        { _fragment = fragment; }

      /// Returns the fragment part of the Uri.
      const std::string& fragment() const
        { return _fragment; }

      /// Returns the Uri as a string.
      ///
      /// All optional parts are left out. The standard ports of http (80), https (443) and ftp (21) are also omitted.
      std::string str() const;
  };
}
}

#endif
