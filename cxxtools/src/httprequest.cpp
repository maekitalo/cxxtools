/* cxxtools/httprequest.cpp
   Copyright (C) 2005 Tommi Maekitalo

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

#include <cxxtools/httprequest.h>

namespace cxxtools
{
  void httprequest::execute()
  {
    if (reading)
    {
      if (peek() != traits_type::eof())
      {
        std::cerr << "reading!" << std::endl;
        return;
      }
      else
        connection.close();
    }

    connection.Connect(host, port);
    switch (method)
    {
      case GET:
        connection << "GET ";
        if (url.size() == 0 || url.at(0) != '/')
          connection << '/';
        connection << url;

        if (!params.empty())
          connection << '?' << params.getUrl();

        connection << " HTTP/1.0\r\n\r\n" << std::flush;
        break;

      case POST:
        {
          std::string p = params.getUrl();

          connection << "POST ";
          if (url.size() == 0 || url.at(0) != '/')
            connection << '/';

          connection << url << " HTTP/1.0\r\n"
                        "Content-Length: " << p.size() << "\r\n"
                        "\r\n"
                     << p << std::flush;
        }
        break;
    }
    reading = true;
  }
}
