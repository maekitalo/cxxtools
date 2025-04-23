#include <cxxtools/bin/rpcserver.h>
#include <cxxtools/bin/rpcclient.h>
#include <cxxtools/remoteprocedureva.h>
#include <cxxtools/eventloop.h>
#include <cxxtools/composer.h>
#include <cxxtools/decomposer.h>
#include <cxxtools/json.h>
#include <cxxtools/arg.h>
#include <cxxtools/log.h>
#include <iostream>
#include <mutex>

static std::mutex clientMutex;

class DynamicComposers : public cxxtools::IComposers
{
public:
    DynamicComposers() = default;

    bool needMore() override    { return false; }
    cxxtools::IComposer* get() override;
    const std::vector<cxxtools::SerializationInfo>& parameters() const { return _parameters; }

private:
    std::vector<cxxtools::Composer<cxxtools::SerializationInfo>> _composers;
    std::vector<cxxtools::SerializationInfo> _parameters;
};

cxxtools::IComposer* DynamicComposers::get()
{
    _composers.emplace_back();
    _parameters.emplace_back();
    _composers.back().begin(_parameters.back());
    return &_composers.back();
}

class ProxyServiceProcedure : public cxxtools::ServiceProcedure
{
    cxxtools::ServiceProcedure* clone() const override;
    cxxtools::IComposers* beginCall(const std::string& function) override;
    cxxtools::IDecomposer* endCall() override;

public:
    ProxyServiceProcedure(cxxtools::RemoteClient& client)
        : _client(client)
        { }

private:
    cxxtools::RemoteClient& _client;
    cxxtools::String _function;
    DynamicComposers _composers;
    cxxtools::Decomposer<cxxtools::SerializationInfo> _r;
};

cxxtools::ServiceProcedure* ProxyServiceProcedure::clone() const
{
    return new ProxyServiceProcedure(_client);
}

cxxtools::IComposers* ProxyServiceProcedure::beginCall(const std::string& function)
{
    _function = cxxtools::String(function);
    return &_composers;
}

cxxtools::IDecomposer* ProxyServiceProcedure::endCall()
{
    std::lock_guard<std::mutex> lock(clientMutex);
    cxxtools::RemoteProcedureVa func(_client, _function);
    std::cout << "call function <" << _function << '>' << std::endl;
    _r.begin(func.call(_composers.parameters()));
    return &_r;
}

int main(int argc, char* argv[])
{
    try
    {
        // initialize logging - this reads the file dynamicprc.properties from the current directory
        log_init(argc, argv);

        cxxtools::Arg<std::string> ip(argc, argv, 'i');
        cxxtools::Arg<unsigned short> port(argc, argv, 'p', 7003);
        cxxtools::Arg<std::string> peerIp(argc, argv, 'I');
        cxxtools::Arg<unsigned short> peerPort(argc, argv, 'P', 7004);

        cxxtools::EventLoop loop;
        cxxtools::bin::RpcServer server(loop, ip, port);
        cxxtools::bin::RpcClient client(peerIp, peerPort);

        std::cout << "rpc proxy listen on port " << port.getValue() << " proxy to port " << peerPort.getValue() << std::endl;

        ProxyServiceProcedure proc(client);
        server.registerDefaultProcedure(proc);

        loop.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

