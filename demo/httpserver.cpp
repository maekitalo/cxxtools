/*
 * Copyright (C) 2009 Tommi Maekitalo
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

#include <cxxtools/http/server.h>
#include <cxxtools/http/request.h>
#include <cxxtools/http/reply.h>
#include <cxxtools/http/responder.h>
#include <cxxtools/eventloop.h>
#include <cxxtools/regex.h>
#include <cxxtools/timer.h>
#include <cxxtools/log.h>
#include <cxxtools/arg.h>

log_define("cxxtools.demo.httpserver")

static cxxtools::EventLoop loop;

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
  log_debug("send hello");

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

// ExitResponder
//
class ExitResponder : public cxxtools::http::Responder
{
    cxxtools::Timer _exitTimer;

  public:
    explicit ExitResponder(cxxtools::http::Service& service)
      : cxxtools::http::Responder(service),
        _exitTimer(&loop)
    {
      cxxtools::connect(_exitTimer.timeout, loop, &cxxtools::EventLoop::exit);
    }

    virtual void reply(std::ostream&, cxxtools::http::Request& request, cxxtools::http::Reply& reply);
};

void ExitResponder::reply(std::ostream& out, cxxtools::http::Request& request, cxxtools::http::Reply& reply)
{
  log_debug("exit server");

  reply.addHeader("Content-Type", "text/html");
  out << "<html>\n"
         " <head>\n"
         "  <title>Hello World-application</title>\n"
         " </head>\n"
         " <body bgcolor=\"#FFFFFF\">\n"
         "  <h1>Exit server</h1>\n"
         " </body>\n"
         "</html>\n";

    log_info("stop server in 1 second");
    _exitTimer.after(cxxtools::Seconds(1));
}

// HelloService
//
typedef cxxtools::http::CachedService<HelloResponder> HelloService;
typedef cxxtools::http::CachedService<ExitResponder> ExitService;

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
    log_init(argc, argv);

    cxxtools::Arg<std::string> listenIp(argc, argv, 'l');
    cxxtools::Arg<unsigned short int> listenPort(argc, argv, 'p', 8001);

    cxxtools::Arg<bool> auth(argc, argv, 'a');

    cxxtools::http::Server server(loop, listenIp, listenPort);

    cxxtools::Arg<unsigned> minThreads(argc, argv, 't', server.minThreads());
    cxxtools::Arg<unsigned> maxThreads(argc, argv, 'T', server.maxThreads());

    server.minThreads(minThreads);
    server.maxThreads(maxThreads);

    // collect additional ports to listen on
    while (true)
    {
        cxxtools::Arg<unsigned short int> listenPort(argc, argv, 'p');
        if (!listenPort.isSet())
            break;

        server.listen(listenIp, listenPort);
    }

    HelloService helloService;
    ExitService exitService;

    MyAuthenticator authenticator;
    if (auth)
      helloService.addAuthenticator(&authenticator);

    server.addService(cxxtools::Regex("ll"), helloService);
    server.addService(cxxtools::Regex("exit"), exitService);
    loop.run();
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

