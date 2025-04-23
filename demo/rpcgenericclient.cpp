#include <iostream>
#include <cxxtools/bin/rpcclient.h>
#include <cxxtools/remoteprocedureva.h>
#include <cxxtools/arg.h>
#include <cxxtools/log.h>
#include <cxxtools/json.h>
#include <cxxtools/sslctx.h>

int main(int argc, char* argv[])
{
    try
    {
        log_init(argc, argv);

        cxxtools::Arg<std::string> ip(argc, argv, 'i');
        cxxtools::Arg<unsigned short> port(argc, argv, 'p', 7003);
        cxxtools::Arg<bool> ssl(argc, argv, 's');
        cxxtools::Arg<bool> secureSsl(argc, argv, 'S');
        cxxtools::Arg<bool> json(argc, argv, 'j');
        cxxtools::Arg<std::string> ciphers(argc, argv, 'C');
        cxxtools::Arg<std::string> domain(argc, argv, 'D');

        if (argc <= 1)
        {
            std::cerr << "usage: " << argv[0] << " {option} function {parameters}\n"
                         "Options:\n"
                         "  -j      pass parameters in json format\n";
            return 1;
        }

        cxxtools::SslCtx sslCtx;
        if (secureSsl)
            sslCtx = cxxtools::SslCtx::secure();
        else if (ssl)
            sslCtx = cxxtools::SslCtx::standard();

        cxxtools::bin::RpcClient client(ip, port, sslCtx);
        client.domain(domain);

        auto function = argv[1];

        std::vector<cxxtools::SerializationInfo> parameters;
        for (int a = 2; a < argc; ++a)
        {
            parameters.emplace_back();
            if (json)
            {
                std::istringstream p(argv[a]);
                p >> cxxtools::Json(parameters.back());
            }
            else
            {
                parameters.back() <<= argv[a];
            }
        }

        cxxtools::RemoteProcedureVa proc(client, function);
        auto result = proc(parameters);
        std::cout << cxxtools::Json(result).beautify(true) << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

