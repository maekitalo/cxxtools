/*
 * Copyright (C) 2015 Tommi Maekitalo
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

#include <iostream>
#include <cxxtools/arg.h>
#include <cxxtools/net/tcpstream.h>
#include <cxxtools/xml/xml.h>
#include <cxxtools/xml/xmlerror.h>
#include "msg.h"

int main(int argc, char* argv[])
{
  try
  {
    cxxtools::Arg<std::string> ip(argc, argv, 'i');
    cxxtools::Arg<unsigned short> port(argc, argv, 'p', 7010);

    // We instantiate a network iostream and connect to a server
    cxxtools::net::TcpStream inp(ip, port);

    cxxtools::xml::XmlDeserializer deserializer;

    // Xml must be read with a XmlReader to prevent, that data is lost
    // in the input stream between messages.
    cxxtools::xml::XmlReader xmlreader(inp);

    // Read messages until we get a XmlNoDocument exception.
    Msg msg;
    while (true)
    {
      try
      {
        deserializer.parse(xmlreader);
      }
      catch (const cxxtools::xml::XmlNoDocument&)
      {
        break;
      }

      deserializer.deserialize(msg);
      std::cout << msg.value << '\t' << msg.str << std::endl;
      deserializer.clear();
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

