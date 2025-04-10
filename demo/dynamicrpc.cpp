#include <cxxtools/bin/rpcserver.h>
#include <cxxtools/eventloop.h>
#include <cxxtools/composer.h>
#include <cxxtools/decomposer.h>
#include <cxxtools/json.h>
#include <cxxtools/arg.h>
#include <cxxtools/log.h>
#include <iostream>

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

class DumperServiceProcedure : public cxxtools::ServiceProcedure
{
    cxxtools::ServiceProcedure* clone() const override;
    cxxtools::IComposers* beginCall(const std::string& function) override;
    cxxtools::IDecomposer* endCall() override;

public:
    std::string _function;
    DynamicComposers _composers;
    cxxtools::Decomposer<int> _r;
};

cxxtools::ServiceProcedure* DumperServiceProcedure::clone() const
{
    return new DumperServiceProcedure();
}

cxxtools::IComposers* DumperServiceProcedure::beginCall(const std::string& function)
{
    _function = function;
    return &_composers;
}

cxxtools::IDecomposer* DumperServiceProcedure::endCall()
{
    // DO dump arguments

    std::cout << "function " << _function << '(';
    bool first = true;
    for (const auto& p: _composers.parameters())
    {
        if (first)
            first = false;
        else
            std::cout << ", ";
        std::cout << cxxtools::Json(p).beautify(true);
    }
    std::cout << ") called" << std::endl;
    _r.begin(1);   // return always 1
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

        cxxtools::EventLoop loop;
        cxxtools::bin::RpcServer server(loop, ip, port);

        DumperServiceProcedure proc;
        server.registerDefaultProcedure(proc);

        loop.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

