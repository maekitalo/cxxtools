RPC with cxxtools
=================

Introduction
------------

In larger systems we often have the requirement to use some functionality on a
different process either on the same host or even via network on another host.
This may be, that we need some data from another process or need to trigger
some action there. For this we need some connection to the other host and a
protocol which specifies, how requests and replies are interpreted.

Cxxtools has a easy to use and flexible rpc framework for that. The framework
defines easy to use classes for server and client sides.

It is not limited to one single rpc protocol but supports currently 4 protocols.
They are:

 * xmlrpc
 * jsonrpc over http
 * jsonrpc raw
 * cxxtools binary protocol

All share a common cxxtools api and it is easy to switch to start with one
protocol and switch another protocol later.

But lets start with one first and come back to the diffenrent protocol later. We
start with xmlrpc since it is most standard and was also the first, which was
supported in cxxtools.

XML-RPC server
-------------

We start by implementing a XML-RPC server. To implement a server is normally
very difficult since we want to be able to handle multiple clients in parallel
without one waiting for the other to complete. But Cxxtools makes it almost
trivial.

So lets define some functions, we want to be called remotely:

    double myAdd(double a, double b)
    {
      return a + b;
    }

    double mySub(double a, double b)
    {
      return a - b;
    }

    double myMul(double a, double b)
    {
      return a * b;
    }

    double myDiv(double a, double b)
    {
      if (b == 0)
        throw std::runtime_error("division by zero");
      return a / b;
    }

Since XML-RPC uses http, we need a http server. This is implemented in the class
cxxtools::http::Server. It needs a event loop, where I/O events and others are
processed. In the main function we have these important lines:

    int main(int argc, char* argv[])
    {
      try
      {
        log_init();
        cxxtools::EventLoop eventLoop;
        cxxtools::http::Server server(eventLoop, 7001);

        eventLoop.run();
      }
      catch (const std::exception& e)
      {
        std::cerr << e.what() << std::endl;
      }
    }

We pass the event loop and port, where the server should accept connections on
to the constructor. By default the server listens on all local interfaces. It is
possible to listen to only one specific local ip address or multiple interfaces
as well. The ip addresses may be IPv4 or IPv6. But we use here just the default.

This call to `log_init()` is optional. I always add it. It looks for a file
`log.xml` in the current directory and initializes logging.

With the proper includes, our http server is already ready to run. But we have
not yet connected the http server to our functions. We have to define which url
to use to call the functions.

And do something magic to call the functions. Note that in XML-RPC we get a
function name and some parameters in XML. Our xmlrpc server has to parse the XML
and extract the function name and parameters and call the function. After that
the return value has to be put back to XML and sent back as a http reply.

Sounds like much work to do. So lets start to do this hard stuff. We define a
service object first:

    cxxtools::xmlrpc::Service service;

We tell connect the service with our functions:

    service.registerFunction("add", myAdd);
    service.registerFunction("sub", mySub);
    service.registerFunction("mul", myMul);
    service.registerFunction("div", myDiv);

And connect the service with the server:

    server.addService("/xmlrpc", service);

This is all done before running the event loop. You won't believe but we are
done. Just put the pieces together, compile the server and we have a working
XML-RPC server with 4 functions. We look at the whole code:

    #include <iostream>
    #include <cxxtools/http/server.h>
    #include <cxxtools/xmlrpc/service.h>
    #include <cxxtools/eventloop.h>
    #include <cxxtools/log.h>

    double myAdd(double a, double b)
    {
      return a + b;
    }

    double mySub(double a, double b)
    {
      return a - b;
    }

    double myMul(double a, double b)
    {
      return a * b;
    }

    double myDiv(double a, double b)
    {
      if (b == 0)
        throw std::runtime_error("division by zero");
      return a / b;
    }

    int main(int argc, char* argv[])
    {
      try
      {
        log_init();
        cxxtools::EventLoop eventLoop;
        cxxtools::http::Server server(eventLoop, 7001);

        cxxtools::xmlrpc::Service service;

        service.registerFunction("add", myAdd);
        service.registerFunction("sub", mySub);
        service.registerFunction("mul", myMul);
        service.registerFunction("div", myDiv);

        server.addService("/xmlrpc", service);

        eventLoop.run();
      }
      catch (const std::exception& e)
      {
        std::cerr << e.what() << std::endl;
      }
    }

Store it in xmlrpcserver.cpp and compile and link it using

    g++ -o xmlrpcserver xmlrpcserver.cpp -lcxxtools -lcxxtools-xmlrpc

So where is all this XML parsing and formatting gone? Everything is done in the
cxxtools::xmlrpc::Service and this magic registerFunction. The registerFunction
is a template function, which generated all the code needed to extract the
parameters and format the return value.

XML-RPC client
-------------

To test our XML-RPC server, we need a client. The client must connect to a
server, create a XML document with a request, send the XML using http to the
server, receive the reply and interpret it. So lets go to work.

Cxxtools has again some helper classes, which makes it very easy. What we need
is a xmlrpc client:

    cxxtools::xmlrpc::HttpClient client("", 7001, "/xmlrpc");

The client gets the ip address, the port and the url as a parameter. We pass a
empty string as ip address, which tells him to connect to the local server.
Depending on the configuration of the machine, IPv4 or IPv6 is used. Both is
already supported by our server.

Next we define a remote procedure class, which does all the formatting stuff for
us. Cxxtools has a template class cxxtools::RemoteProcedure with a variable
number of template parameters. The first template parameter defines the type of
the return value and the rest the type of the parameters. The class expects the
client and the function name as parameter to the constructor:

    cxxtools::RemoteProcedure<double, double, double> add(client, "add");

Believe it or not - we are already done. We are now able to call our remote
function. The RemoteProcedure object has a operator (), which takes the
parameters we specified. So to add the numbers 12 and 17 on the server, we just
use the operator like this:

    double result = add(12, 17);

To put everything together we need some includes and a proper main function.
I added the rest of our functions to our example. Here is the complete code:

    #include <iostream>
    #include <cxxtools/xmlrpc/httpclient.h>
    #include <cxxtools/remoteprocedure.h>
    #include <cxxtools/log.h>

    int main(int argc, char* argv[])
    {
      try
      {
        log_init();
        cxxtools::xmlrpc::HttpClient client("", 7001, "/xmlrpc");
        cxxtools::xmlrpc::RemoteProcedure<double, double, double> add(client, "add");
        cxxtools::xmlrpc::RemoteProcedure<double, double, double> sub(client, "sub");
        cxxtools::xmlrpc::RemoteProcedure<double, double, double> mul(client, "mul");
        cxxtools::xmlrpc::RemoteProcedure<double, double, double> div(client, "div");

        std::cout << "4 + 5 = " << add(4, 5) << std::endl;
        std::cout << "4 - 5 = " << sub(4, 5) << std::endl;
        std::cout << "4 * 5 = " << mul(4, 5) << std::endl;
        std::cout << "4 / 5 = " << div(4, 5) << std::endl;
      }
      catch (const std::exception& e)
      {
        std::cerr << e.what() << std::endl;
      }
    }

Store it in a file named xmlrpcclient.cpp and compile it using:

    g++ -o xmlrpcclient xmlrpcclient.cpp -lcxxtools -lcxxtools-xmlrpc

Now we are ready to run both. Start a second terminal and run the server in one
terminal using:

    ./xmlrpcserver

And in the other terminal run the client with:

    ./xmlrpcclient

We should see the results of the calculations.

Helpful is to enable some log output. Run the command:

    cxxtools-config --logxml xmlrpc >log.xml

To create a basic configuration for logging. If you run the server, you will see
some log output and also each request from the client.

XML-RPC error handling
---------------------

When we look to our server code, we can see, that the function "myDiv", which
was exported as "div" to our client throws an exception when the divisor is 0.
Try it by changing the client to pass a 0. What we see, that we catch the
exception really on the client size. Cxxtools makes it really transparent. The
only difference is, that the type of the exception is really lost. The type of
the exception on the client side is always `cxxtools::RemoteException`, which is
derived from std::exception. Just the text of the exception is preserved.

If you look at the definition of `cxxtools::RemoteException`, you may see, that
there is a optional return code. This is also preserved. So if you really throw
a `cxxtools::RemoteException` on the server side with a error code, you can see
the error code on the client side.

Note that an exception is also thrown on techical errors, e.g. when the client
could not connect to the server.

Serialization
-------------

The world is never perfect and so rpc is not. There are indeed limitations.
XML-RPC can just send data. You cannot send arbitrary objects like file
descriptors on window handles. So lets look at the possible parameter types.

When you look at the XML-RPC specification, it can send scalars, structs and
arrays. Somehow the XML-RPC implementation of cxxtools has to know, how to
convert C++ data to XML and back.


Other protocols
---------------

Reconnection, keep alive
------------------------
