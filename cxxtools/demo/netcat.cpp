#include <exception>
#include <iostream>
#include <cxxtools/tcpstream.h>
#include <cxxtools/arg.h>

int main(int argc, char* argv[])
{
  try
  {
    cxxtools::arg<const char*> ip(argc, argv, 'i', "0.0.0.0");
    cxxtools::arg<unsigned short> port(argc, argv, 'p', 1234);

    // listen to a port
    cxxtools::tcp::Server server(ip, port);

    // accept a connetion
    cxxtools::tcp::iostream worker(server);

    // copy to stdout
    std::cout << worker.rdbuf();
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

