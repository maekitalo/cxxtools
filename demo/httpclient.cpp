/*
 * Copyright (C) 2014 Tommi Maekitalo
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

/*
   This demo is a very simple http client. It just fetches one or more pages
   from a web server and prints the content.

   It is called with one or more http requests as parameters.
 */

#include <iostream>
#include <cxxtools/http/client.h>
#include <cxxtools/net/uri.h>
#include <cxxtools/query_params.h>

int main(int argc, char* argv[])
{
  try
  {
    // We default construct a http client
    cxxtools::http::Client client;

    for (int a = 1; a < argc; ++a)
    {
      // This parses the argument, e.g. "http://www.tntnet.org/cxxtools.html"
      // See documentation of cxxtools::net::Uri for details
      cxxtools::net::Uri uri(argv[a]);

      // This passes the host and port to the http client.
      // Note that in case the client is connected already and the same host
      // and port is specified, the connection is kept open.
      client.prepareConnect(uri);

      // Fetch the content from the web server and print out.
      std::cout << client.get(uri.path(), cxxtools::QueryParams(uri.query())).body();
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

