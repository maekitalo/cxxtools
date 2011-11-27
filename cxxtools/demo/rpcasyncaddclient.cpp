/*
 * Copyright (C) 2011 Tommi Maekitalo
 *
 */

#include <iostream>
#include <cxxtools/arg.h>
#include <cxxtools/log.h>
#include <cxxtools/xmlrpc/httpclient.h>
#include <cxxtools/bin/rpcclient.h>
#include <cxxtools/remoteprocedure.h>
#include <cxxtools/eventloop.h>

cxxtools::EventLoop loop;

void onFinished(const cxxtools::RemoteResult<double>& result)
{
  try
  {
    double r = result.get(); // This may throw an exception, which may have
                             // happened on the server method, but also local
                             // errors like "connection refused" are reported
                             // here.

    std::cout << "result=" << r << std::endl;
  }
  catch (const std::exception& e)
  {
    std::cout << "failed with error: " << e.what() << std::endl;
  }

  // exit the main event loop
  loop.exit();
}

int main(int argc, char* argv[])
{
  try
  {
    log_init();

    cxxtools::Arg<std::string> ip(argc, argv, 'i');
    cxxtools::Arg<bool> binary(argc, argv, 'b');
    cxxtools::Arg<unsigned short> port(argc, argv, 'p', binary ? 7003 : 7002);

    // define a xlmrpc client
    cxxtools::xmlrpc::HttpClient xmlrpcClient(loop, ip, port, "/myservice");
    // and a binary rpc client
    cxxtools::bin::RpcClient binaryClient(loop, ip, port);

    // define remote procedure with dobule return value and two double parameter:
    cxxtools::RemoteProcedure<double, double, double> add(
        binary ? static_cast<cxxtools::RemoteClient&>(binaryClient) :
                 static_cast<cxxtools::RemoteClient&>(xmlrpcClient), "add");

    // connect the callback method to our method
    cxxtools::connect(add.finished, onFinished);

    // request the execution of our method
    add.begin(5, 6);

    // Run the loop, which sends the request to the server and receives the answer.
    // After receiving the answer, the callback onFinished is called.
    //
    // Note that you may have multiple rpc clients running on the loop on the same time
    // as well as other async stuff, like a rpc server.
    loop.run();
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

