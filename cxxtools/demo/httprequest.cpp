#include <exception>
#include <iostream>
#include <cxxtools/httprequest.h>
#include <cxxtools/arg.h>

int main(int argc, char* argv[])
{
  try
  {
    cxxtools::arg<unsigned short> port(argc, argv, 'p', 80);
    cxxtools::arg<bool> raw(argc, argv, 'r');

    cxxtools::arg<std::string> host("127.0.0.1");
    host.set(argc, argv);

    cxxtools::arg<std::string> url("/");
    url.set(argc, argv);

    // set parameters
    cxxtools::httprequest request(host, port, url);

    // execute request
    if (raw)
    {
      // execute request and read result including http-headers
      request.execute();
      std::cout << request.rdbuf() << std::flush;
    }
    else
    {
      // let httpreply parse the http-headers and read only body
      cxxtools::httpreply reply(request);
      std::cout << "return-code " << reply.getReturnCode() << '\n'
                << reply.rdbuf() << std::flush;
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

