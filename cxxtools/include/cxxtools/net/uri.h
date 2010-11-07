/*
 * Copyright (C) 2010 Tommi Maekitalo
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

#include <string>

namespace cxxtools
{
namespace net
{

  class Uri
  {
      bool _ipv6;
      std::string _protocol;
      std::string _user;
      std::string _password;
      std::string _host;
      unsigned short int _port;
      std::string _url;

      void throwInvalid(const std::string& uri) const;

    public:
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

      void url(const std::string& url)
        { _url = url; }

      const std::string& url() const
        { return _url; }

      std::string str() const;
  };
}
}
