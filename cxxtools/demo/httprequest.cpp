#include <exception>
#include <iostream>
#include <cxxtools/httprequest.h>
#include <cxxtools/arg.h>

int main(int argc, char* argv[])
{
  try
  {
    cxxtools::arg<unsigned short> port(argc, argv, 'p', 80);

    cxxtools::arg<std::string> host("127.0.0.1");
    host.set(argc, argv);

    cxxtools::arg<std::string> url("/");
    url.set(argc, argv);

    // set parameters
    cxxtools::httprequest request(host, port, url);

    // execute request
    request.execute();

    std::cout << request.rdbuf() << std::flush;
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

