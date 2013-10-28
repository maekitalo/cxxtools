JSON RPC howto
==============

Introduction
------------

JSON RPC is a protocol, which which uses the JSON data format to call procedures
on a remote server over the network.

`Cxxtools` has implementations for JSON RPC clients and and servers. `Cxxtools`
makes it really easy to call remote JSON RPC procedures or to implement
procedures, which can be called over the network.

JSON RPC can be used raw or over a http frame. Both variants are implemented in
`cxxtools`.

JSON RPC server
---------------

We start with a simple JSON RPC server, which makes a function available to a
remote side. The example uses JSON RPC without HTTP.

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

            // Define a json rpc server and listen on port 7077 (on all interfaces):
            cxxtools::json::RpcServer jsonServer(loop, 7077);

            // register the methods
            jsonServer.registerFunction("add", add);
            jsonServer.registerFunction("sub", sub);

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
    #include <cxxtools/json/rpcserver.h>

This was easy. Note that the `registerFunction` method of the server is a
template function, which generates all the stuff needed to convert the
parameters from json to the needed types and the result back to json.

The parameters and return values must have a cxxtools serialization and
deserialization operator. All standard types including container classes in the
std namespace have one and it is easy to implement one for own types.

See the serialization howto for details.

The server is multithreaded and starts by default 5 threads at startup.

JSON RPC client
---------------

Next we want to implement a client, which calls our methods on the server. It is
even simpler than the server:

    // Define client class with a empty IP, which means localhost and a port
    cxxtools::json::RpcClient client("", 7077);

    // Define the remote procedure with 3 template parameters. The first is
    // always the return type and the rest are the types of the parameters.
    cxxtools::RemoteProcedure<double, double, std::string> add(client, "add");

    // and call the procedure:
    double result = add(17, 4);
    std::cout << "result=" << result << std::endl;

Very simple. Again `cxxtools` uses template magic in `cxxtools::RemoteProcedure`
to generate all the serialization and deserialization stuff we need.

To make the example complete we need the headers:

    #include <cxxtools/remoteprocedure.h>
    #include <cxxtools/json/rpcclient.h>

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

Make JSON RPC client asynchronous
---------------------------------

The call to the `add` procedure is done synchronous, that means, that the call
`add(17, 4)` blocks until the json rpc call returns from the server. Sometimes
it would be nice, if we can run multiple rpc calls in parallel. Luckily
`cxxtools` supports that.

There are 2 options doing that. The most flexible but also more difficult way to
do it, is to run the procedure in a event loop. For that we have to register
callbacks which inform the client, when the call is finished. But this is not in
scope of this howto.

Lets look at the simpler one. We just want initiate multiple calls and wait
until all have finished.

The following example initiate calls to the `add` and `sub` methods and waits
until the procedures return.

    // We need a selector, which is a abstraction of poll(2) but easier to use
    cxxtools::Selector selector;

    // One client can only run one procedure at a time so to run 2 in parallel
    // we need 2 clients. We pass the selector as the first argument to the
    // client:
    cxxtools::json::RpcClient jsonClientAdd(selector, "", 7077);
    cxxtools::json::RpcClient jsonClientSub(selector, "", 7077);

    // We also need remote procedures for both methods:
    cxxtools::RemoteProcedure<double, double, std::string> add(jsonClientAdd, "add");
    cxxtools::RemoteProcedure<double, double, std::string> sub(jsonClientSub, "sub");

    // Initiate the calls; note that the calls are not sent to the server until
    // we start the selector:
    add.begin(17, 4);
    sub.begin(17, 4);

    // Call wait on the selector until the add method is finished.
    // note that since both procedures run in the same selector, the sub is also
    // executed.
    while (add.activeProcedure() != 0)
        selector.wait();

    std::cout << "add returns " << add.result() << std::endl;

    // Wait until sub is also finished. It may be finished immediately since the
    // selector was run already.
    while (sub.activeProcedure() != 0)
        selector.wait();

    std::cout << "sub returns " << sub.result() << std::endl;

We need additionally the header:

    #include <cxxtools/selector.h>

The remote procedures may be located also on different servers. Also any number
of clients can be executed in parallel.

There is a event shorter way to write that in `cxxtools`. Each remote procedure
has a method `end()` which executes the selector until the remote procedure
finishes and returns the result of the procedure. So calling `add.end()` will
block until the `add` procedure is finished.

Note that since `add.end()` just processes the I/O events by calling wait on the
selector it runs actually the `sub` procedure also. It may well be, that `sub`
is finished before `add` and hence fully executed by `add.end()`. Calling
`sub.end()` later will then just return the result.

So instead of the while loop and fetching the result with the `result` method we
can just write:

    add.begin(17, 4);
    sub.begin(17, 4);

    std::cout << "add returns " << add.end() << std::endl;
    std::cout << "sub returns " << sub.end() << std::endl;

Note that even when `add.end()` executes `sub` also, exceptions thrown by the
`sub` procedure on the server side are thrown always when calling `sub.end()`.
What may still happen is, that `add.end()` throws I/O errors which were caused
by `sub`.

JSON RPC over http server
-------------------------

As noted before `cxxtools` supports JSON RPC raw as well as over http. We change
our server to use http.

    // Define a event loop
    cxxtools::EventLoop loop;

    // Define a http server first
    cxxtools::http::Server httpServer(loop, 8077);

    // for json rpc over http we need a service object
    cxxtools::json::HttpService service;

    // we register our functions
    service.registerFunction("add", add);
    service.registerFunction("sub", sub);

    // ... and register the service under a url
    httpServer.addService("/jsonrpc", service);

    // and run our server
    loop.run();

To make the example complete we need the headers:

    #include <cxxtools/eventloop.h>
    #include <cxxtools/http/server.h>
    #include <cxxtools/json/httpservice.h>

We already see, that http gives us one more level. We can register different
services under different urls. They act like a namespace in json rpc.

The http server also supports other types of services. We can for example run a
xmlrpc service on the very same http server under a different url. Note that
xmlrpc is supported by cxxtools also.

If you want to use xmlrpc, you can just replace json with xmlrpc in the above
example and it uses a different protocol. Nothing else need to be changed.

JSON RPC over http client
-------------------------

To call the json rpc procedure over http we need a change on our client side as
well. Here is the example:

    // Define client class with a empty IP, which means localhost and a port.
    cxxtools::json::HttpClient client("", 7077, "/jsonrpc");

    // The remote procedure class is defined like before.
    cxxtools::RemoteProcedure<double, double, std::string> add(client, "add");

    // and neither the call has changed in any way:
    double result = add(17, 4);
    std::cout << "result=" << result << std::endl;

To make the example complete we need one additional header:

    #include <cxxtools/json/httpclient.h>

Like the server the client also supports xmlrpc. Just replace again json with
xmlrpc and the protocol of the client is changed.

Using complex structures in JSON RPC
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
