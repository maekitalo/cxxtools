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

#include <cxxtools/api.h>
#include <string>

namespace cxxtools
{
namespace net
{

  class CXXTOOLS_API Uri
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

    public:
      Uri()  { }

      Uri(const std::string& str);

      void protocol(const std::string& protocol)
        { _protocol = protocol; }

      const std::string& protocol() const
        { return _protocol; }

      void user(const std::string& user)
        { _user = user; }

      const std::string& user() const
        { return _user; }

      void password(const std::string& password)
        { _password = password; }

      const std::string& password() const
        { return _password; }

      void host(const std::string& host)
        { _host = host; }

      const std::string& host() const
        { return _host; }

      void port(unsigned short int p)
        { _port = p; }

      unsigned short int port() const
        { return _port; }

      void path(const std::string& path)
        { _path = path; }

      const std::string& path() const
        { return _path; }

      void query(const std::string& query)
        { _query = query; }

      const std::string& query() const
        { return _query; }

      void fragment(const std::string& fragment)
        { _fragment = fragment; }

      const std::string& fragment() const
        { return _fragment; }

      std::string str() const;
  };
}
}
