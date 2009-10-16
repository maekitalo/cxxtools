/*
 * Copyright (C) 2009 Tommi Maekitalo
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

#include <cxxtools/http/server.h>
#include <cxxtools/http/request.h>
#include <cxxtools/http/reply.h>
#include <cxxtools/http/responder.h>
#include <cxxtools/loginit.h>
#include <cxxtools/arg.h>

// HelloResponder
//
class HelloResponder : public cxxtools::http::Responder
{
  public:
    explicit HelloResponder(cxxtools::http::Service& service)
      : cxxtools::http::Responder(service)
      { }

    virtual void reply(std::ostream&, cxxtools::http::Request& request, cxxtools::http::Reply& reply);
};

void HelloResponder::reply(std::ostream& out, cxxtools::http::Request& request, cxxtools::http::Reply& reply)
{
  reply.addHeader("Content-Type", "text/html");
  out << "<html>\n"
         " <head>\n"
         "  <title>Hello World-application</title>\n"
         " </head>\n"
         " <body bgcolor=\"#FFFFFF\">\n"
         "  <h1>Hello World</h1>\n"
         " </body>\n"
         "</html>\n";

}

// HelloService
//
typedef cxxtools::http::CachedService<HelloResponder> HelloService;

// implement authenticator
//
class MyAuthenticator : public cxxtools::http::Authenticator
{
  public:
    virtual bool checkAuth(const cxxtools::http::Request&) const;
};

bool MyAuthenticator::checkAuth(const cxxtools::http::Request& request) const
{
  cxxtools::http::Request::Auth auth = request.auth();

  if (auth.user != "cxxtools" || auth.password != "mypassword")
    return false;

  return true;
}

// main
//
int main(int argc, char* argv[])
{
  try
  {
    log_init();

    cxxtools::Arg<std::string> listenIp(argc, argv, 'l', "0.0.0.0");
    cxxtools::Arg<unsigned short int> listenPort(argc, argv, 'p', 8001);
    cxxtools::Arg<bool> auth(argc, argv, 'a');

    cxxtools::http::Server server(listenIp, listenPort);
    HelloService service;

    MyAuthenticator authenticator;
    if (auth)
      service.addAuthenticator(&authenticator);

    server.addService("/hello", service);
    server.run();
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

