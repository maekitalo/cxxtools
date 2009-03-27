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

#include <vector>
#include <cxxtools/httpserver.h>
#include <cxxtools/httpreply.h>
#include <cxxtools/mutex.h>
#include <cxxtools/eventloop.h>
#include <cxxtools/loginit.h>

// HelloResponder
//
class HelloResponder : public cxxtools::net::HttpResponder
{
  public:
    explicit HelloResponder(cxxtools::net::HttpService& service)
      : cxxtools::net::HttpResponder(service)
      { }

    virtual void reply(std::ostream&, cxxtools::net::HttpRequest& request, cxxtools::net::HttpReply& reply);
};

void HelloResponder::reply(std::ostream& out, cxxtools::net::HttpRequest& request, cxxtools::net::HttpReply& reply)
{
  reply.addHeader("Content-Type", "text/html");
  out << "<html>\n"
         " <head>\n"
         "  <title>Hello World-application for tntnet</title>\n"
         " </head>\n"
         "\n"
         " <body bgcolor=\"#FFFFFF\">\n"
         "  <img src=\"tntnet.jpg\" align=\"right\">\n"
         "\n"
         "  <h1>Hello World</h1>\n"
         "\n"
         "  <form>\n"
         "   What's your name?\n"
         "   <input type=\"text\" name=\"name\" value=\"\"> <br>\n"
         "   <input type=\"submit\">\n"
         "  </form>\n"
         "\n"
         " </body>\n"
         "</html>\n\n\n";

}

// HelloService
//
class HelloService : public cxxtools::net::HttpService
{
    cxxtools::Mutex mutex;
    typedef std::vector<cxxtools::net::HttpResponder*> Responders;
    Responders responders;

  public:
    ~HelloService();
    virtual cxxtools::net::HttpResponder* createResponder(const cxxtools::net::HttpRequest&);
    virtual void releaseResponder(cxxtools::net::HttpResponder*);
};

HelloService::~HelloService()
{
  for (Responders::iterator it = responders.begin(); it != responders.end(); ++it)
    delete *it;
}

cxxtools::net::HttpResponder* HelloService::createResponder(const cxxtools::net::HttpRequest&)
{
  cxxtools::MutexLock lock(mutex);
  if (responders.empty())
  {
    return new HelloResponder(*this);
  }
  else
  {
    cxxtools::net::HttpResponder* ret = responders.back();
    responders.pop_back();
    return ret;
  }
}

void HelloService::releaseResponder(cxxtools::net::HttpResponder* resp)
{
  responders.push_back(resp);
}

// main
//
int main(int argc, char* argv[])
{
  try
  {
    log_init();

    cxxtools::net::HttpServer server("0.0.0.0", 8001);
    HelloService service;
    server.addService("/hello", service);
    server.run();
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

