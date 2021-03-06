#include <iostream>
#include <cxxtools/arg.h>
#include <cxxtools/log.h>
#include <cxxtools/sslcertificate.h>
#include <cxxtools/remoteprocedure.h>
#include <cxxtools/bin/rpcclient.h>

bool noAccept(const cxxtools::SslCertificate& cert)
{
    std::cout << "cert \"" << cert.getSubject() << "\" not accepted" << std::endl;
    return false;
}

////////////////////////////////////////////////////////////////////////
// main
//
int main(int argc, char* argv[])
{
  try
  {
    log_init();

    cxxtools::Arg<std::string> ip(argc, argv, 'i');
    cxxtools::Arg<unsigned short> port(argc, argv, 'p', 7003);
    cxxtools::Arg<std::string> cert(argc, argv, 'c');
    cxxtools::Arg<std::string> ca(argc, argv, 'C');

    cxxtools::bin::RpcClient client(ip, port, cert);

    if (ca.isSet())
        client.setSslVerify(2, ca);

    cxxtools::RemoteProcedure<std::string, std::string> echo(client, "echo");

    for (int a = 1; a < argc; ++a)
    {
      std::string v = echo(argv[a]);
      std::cout << v << '\n';
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

