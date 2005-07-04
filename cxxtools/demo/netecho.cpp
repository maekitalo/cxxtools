#include <exception>
#include <iostream>
#include <cxxtools/tcpstream.h>
#include <cxxtools/arg.h>

int main(int argc, char* argv[])
{
  try
  {
    cxxtools::arg<const char*> ip(argc, argv, 'i', "127.0.0.1");
    cxxtools::arg<unsigned short> port(argc, argv, 'p', 1234);
    cxxtools::arg<bool> read_reply(argc, argv, 'r');
    cxxtools::arg<unsigned> bufsize(argc, argv, 'b', 8192);

    // connect to server
    cxxtools::tcp::iostream peer(ip, port, bufsize);

    // copy stdin to server
    peer << std::cin.rdbuf() << std::flush;

    if (read_reply)
      // copy answer to stdout
      std::cout << peer.rdbuf() << std::flush;
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

