#include <cxxtools/remoteprocedureva.h>
#include <cxxtools/bin/rpcclient.h>
#include <cxxtools/arg.h>
#include <cxxtools/log.h>
#include <iostream>

int main(int argc, char* argv[])
{
    try
    {
        log_init(argc, argv);

        cxxtools::Arg<std::string> ip(argc, argv, 'i');
        cxxtools::Arg<unsigned short> port(argc, argv, 'p', 7003);

        cxxtools::bin::RpcClient client(ip, port);
        cxxtools::RemoteProcedureVa<cxxtools::SerializationInfo> proc(client, "proc");

        proc(17, 24);
        proc("Hi");
        proc("Hello", "World");
        proc("A value", 3.14159);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}
