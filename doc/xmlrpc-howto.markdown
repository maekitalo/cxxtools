XML RPC howto
=============

This document is derived from the json rpc howto.

Introduction
------------

XML RPC is a protocol, which which uses XML to call procedures on a remote
server over the network.

`Cxxtools` has implementations for XML RPC clients and and servers. `Cxxtools`
makes it really easy to call remote JSON RPC procedures or to implement
procedures, which can be called over the network.

XML RPC always run on a http server. `cxxtools` provides the server, needed to
run the service.

XML RPC server
---------------

We start with a simple XML RPC server, which makes a function available to a
remote side.

First implement the functions, which should be callable:

    double add(double a1, double a2)
    {
        return a1 + a2;
    }

    double sub(double a1, double a2)
    {
        return a1 - a2;
    }

In our main function we make the functions available over the network:

    int main(int argc, char* argv[])
    {
        try
        {
            // Define a event loop
            cxxtools::EventLoop loop;

            // Define a http server first
            cxxtools::http::Server httpServer(loop, 8077);

            // for xml rpc we need a service object
            cxxtools::xmlrpc::Service service;

            // register the methods
            service.registerFunction("add", add);
            service.registerFunction("sub", sub);

            // ... and register the service under a url
            httpServer.addService("/xmlrpc", service);

            // and run our server
            loop.run();
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }

To make the example complete we need the headers:

    #include <cxxtools/eventloop.h>
    #include <cxxtools/http/server.h>
    #include <cxxtools/xmlrpc/service.h>

This was easy. Note that the `registerFunction` method of the service is a
template function, which generates all the stuff needed to convert the
parameters from xml to the needed types and the result back to xml.

The parameters and return values must have a cxxtools serialization and
deserialization operator. All standard types including container classes in the
std namespace have one and it is easy to implement one for own types.

We show a simple example for extending custom types later.

The server is multithreaded and starts by default 5 threads at startup.

XML RPC client
---------------

Next we want to implement a client, which calls our methods on the server. It is
even simpler than the server:

    // Define client class with a empty IP, which means localhost and a port
    cxxtools::xmlrpc::HttpClient client("", 8077, "/xmlrpc);

    // Define the remote procedure with 3 template parameters. The first is
    // always the return type and the rest are the types of the parameters.
    cxxtools::RemoteProcedure<double, double, std::string> add(client, "add");

    // and call the procedure:
    double result = add(17, 4);
    std::cout << "result=" << result << std::endl;

To make the example complete we need the headers:

    #include <cxxtools/remoteprocedure.h>
    #include <cxxtools/xmlrpc/httpclient.h>

Very simple. Again `cxxtools` uses template magic in `cxxtools::RemoteProcedure`
to generate all the serialization and deserialization stuff we need.
Handling exceptions
-------------------

There were 2 types of exceptions, which can happen here. Either purely technical
exceptions like I/O errors when the server is not reachable or exceptions thrown
by the procedure on the server side.

Technical exceptions are handled like always in `cxxtools`. They are just
thrown. There is nothing special here.

Exceptions thrown by the server procedure are sent to the client. The client
then throws the exception when the result is requested. So it behaves like
a local function call. The only difference is, that the actual exception type is
lost. The exception is always derived from `cxxtools::RemoteException`.

Note that all exceptions thrown in cxxtools are derived from `std::exception`.
So as shown in the example it is sufficient to catch just `std::exception` to
catch all exceptions of the remote procedure.


Using complex structures in XML RPC
------------------------------------

We already mentioned, that the serialization framework of cxxtools is used. We
will look here at a simple example, how to make a struct usable in rpc.

So this is our struct, we want to use as a parameter or return value in our RPC
function:

    struct MyData
    {
        double someValue;
        bool aFlag;
        std::vector<int> moreValues;
    };

We need 2 operators:

    void operator<<= (cxxtools::SerializationInfo& si, const MyData& myData)
    {
        si.addMember("someValue") <<= myData.someValue;
        si.addMember("aFlag") <<= myData.aFlag;
        si.addMember("moreValues") <<= myData.moreValues;
    }

    void operator>>= (const cxxtools::SerializationInfo& si, MyData& myData)
    {
        si.getMember("someValue") >>= myData.someValue;
        si.getMember("aFlag") >>= myData.aFlag;
        si.getMember("moreValues") >>= myData.moreValues;
    }

To define the operators a header is needed:

    #include <cxxtools/serializationinfo.h>

And we are done. Now we can use the structure just like any other type.

Note that in C++ we usually have more than one source file and structures are
defined in separate headers. The operators must then either be defined in the
header as inline functions or just declared in the header and defined in a cpp
class, which is compiled separately.

For data we usually use classes, with private members. A good choice is to
declare the operators as friends, so that they can easily access the private
members.
