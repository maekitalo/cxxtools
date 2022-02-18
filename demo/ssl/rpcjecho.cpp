#include <iostream>
#include <cxxtools/arg.h>
#include <cxxtools/log.h>
#include <cxxtools/remoteprocedure.h>
#include <cxxtools/json/rpcclient.h>
#include <cxxtools/sslctx.h>

////////////////////////////////////////////////////////////////////////
// main
//
int main(int argc, char* argv[])
{
  try
  {
    log_init();

    cxxtools::Arg<std::string> ip(argc, argv, 'i');
    cxxtools::Arg<unsigned short> port(argc, argv, 'p', 7004);
    cxxtools::Arg<std::string> cert(argc, argv, 'c');
    cxxtools::Arg<std::string> ca(argc, argv, 'C');

    cxxtools::SslCtx sslCtx;

    if (cert.isSet())
        sslCtx.loadCertificateFile(cert);

    if (ca.isSet())
        sslCtx.setVerify(2, ca);

    cxxtools::json::RpcClient client(ip, port, sslCtx);

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

