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

But lets start with one first and come back to the different protocol later. We
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
        cxxtools::RemoteProcedure<double, double, double> add(client, "add");
        cxxtools::RemoteProcedure<double, double, double> sub(client, "sub");
        cxxtools::RemoteProcedure<double, double, double> mul(client, "mul");
        cxxtools::RemoteProcedure<double, double, double> div(client, "div");

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

Note that an exception is also thrown on technical errors, e.g. when the client
could not connect to the server.

Serialization
-------------

The world is never perfect and so rpc is not. There are indeed limitations in
rpc calls. XML-RPC can just send data. You cannot send arbitrary objects like
file descriptors or window handles. So lets look at the possible parameter
types.

When you look at the XML-RPC specification, it can send scalars, structs and
arrays. Somehow the XML-RPC implementation of `cxxtools` has to know, how to
convert C++ data to XML and back. This is called serialization and
deserialization.

There is a separate document about the details about serialization, so lets
concentrate on the bare minimum.

`cxxtools` has a special data structure called `cxxtools::SerializationInfo`.
This class holds the information to generate xml and xml is converted back to
this structure. What must be done is to teach cxxtools, how to convert our
parameters to SerializationInfo and back.

Luckily this is already done for all built in types including standard library
containers. That's why our examples above worked out of the box.

So what needs to be done for custom data? We need to define 2 operators. We go
straight away to a example:

    struct MyStruct
    {
      std::string stringValue;
      int intValue;
      std::vector<double> doubleValues;
    };

We want to use that structure as a parameter (or return type) in a XML-RPC
function. We add 2 operators:

    void operator<<= (cxxtools::SerializationInfo& si, const MyStruct& ms)
    {
      si.addMember("stringValue") <<= ms.stringValue;
      si.addMember("intValue") <<= ms.intValue;
      si.addMember("doubleValues") <<= ms.doubleValues;
    }

    void operator>>= (const cxxtools::SerializationInfo& si, MyStruct& ms)
    {
      si.getMember("stringValue") >>= ms.stringValue;
      si.getMember("intValue") >>= ms.intValue;
      si.getMember("doubleValues") >>= ms.doubleValues;
    }

The first operator tells, how to convert a `MyStruct` into
`cxxtools::SerializationInfo` and the other converts it back. This is all we
have to do.

You can see, that the struct has members, which are all standard types, so
cxxtools already knows how to handle them for XML-RPC.

Note that as always in C++ you have to take care not to have duplicate symbols.
The operators are normally either declared in the header and defined in a cpp
file or if fully defined in the header marked as inline.

Now we can define a function e.g.:

    MyStruct getMyStruct()
    {
      MyStruct s;
      s.stringValue = "Hi";
      s.intValue = "Hi";
      s.doubleValues.push_back(17);
      s.doubleValues.push_back(42);
    }

and make them available as a XML-RPC service just like our previous myAdd,
mySub, myMul and myDiv functions. We do not show a full example here, since it
duplicates most of the code from the above example hence can be easily derived
from the first example.

Of course you have to make sure both client and server know about the structure
we defined.

Other protocols
---------------

As already mentioned cxxtools is not limited to XML-RPC. XML-RPC is a good
choice since it is simple and flexible. The downside is, that it is quite
verbose. The other 3 protocols are faster. Lets look at the other ones and how
to implement them in cxxtools.

### jsonrpc over http

The most similar protocol is jsonrpc over http since it uses the same transport
protocol http as XML-RPC. To change our server to speak jsonrpc over http there
is just two lines to change. The first line is the include. Instead of including
`<cxxtools/xmlrpc/service.h>` we need the header
`<cxxtools/json/httpservice.h>`. The other line is the service object. Instead
of `cxxtools::xmlrpc::Service` we use the type `cxxtools::json::HttpService`.
And to be clean we should also change the url from "/xmlrcp" to "/jsonrpc".

Here is our full server which talks jsonrpc over http:

    #include <iostream>
    #include <cxxtools/http/server.h>
    #include <cxxtools/json/httpservice.h>
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

        cxxtools::json::HttpService service;

        service.registerFunction("add", myAdd);
        service.registerFunction("sub", mySub);
        service.registerFunction("mul", myMul);
        service.registerFunction("div", myDiv);

        server.addService("/jsonrpc", service);

        eventLoop.run();
      }
      catch (const std::exception& e)
      {
        std::cerr << e.what() << std::endl;
      }
    }

There are indeed only 3 lines changed. To build the server, store the code in
jsonhttpserver.cpp and compile and link it using:

    g++ -o jsonhttpserver jsonhttpserver.cpp -lcxxtools -lcxxtools-json

We need to link against cxxtools-json instead of cxxtools-xmlrpc.

Since both protocols share the same http server but use different urls, you can
even combine both, so that the server can talk both protocols. What need to be
done is just to define both services and add them both to the http server.

But one piece is missing before we can test our jsonrpc over http server: the
client. You can guess it - it is just as easy to change the client to talk
jsonrpc over http instead of XML-RPC as the server. Or even easier since there
are only 2 lines of code to change. We change the include from
`<cxxtools/xmlrpc/httpclient.h>` to `<cxxtools/json/httpclient.h>` and the
definition of our client to:

    cxxtools::json::HttpClient client("", 7001, "/jsonrpc");

That is all we have to do. We do not show the full example here since it is so
trivial and a good exercise.

### jsonrpc raw

The next supported protocol is jsonrpc raw. It do not use http as a transport
protocol but uses just a socket connection and hence even more compact.

This time we do not instantiate a http server but a jsonrpc server. Also we do
not have a service object but register our functions directly to the server.

The server is defined in the header `<cxxtools/json/rpcserver.h>`.

We do not show the full code here because again it is almost identical to the
above examples. Here is just the main function of our jsonrpc server:

    int main(int argc, char* argv[])
    {
      try
      {
        log_init();
        cxxtools::EventLoop eventLoop;
        cxxtools::json::RpcServer server(eventLoop, 7001);

        server.registerFunction("add", myAdd);
        server.registerFunction("sub", mySub);
        server.registerFunction("mul", myMul);
        server.registerFunction("div", myDiv);

        eventLoop.run();
      }
      catch (const std::exception& e)
      {
        std::cerr << e.what() << std::endl;
      }
    }

There is not too much new here. It is even simpler since the lack of the service
object.

The client is again trivial to change. The client is defined in the header
`<cxxtools/json/rpcclient.h>`. To define a client we use:

    cxxtools::json::RpcClient client("", 7001);

Not that we do not have a service url.

### cxxtools binary protocol

If you do not care about compatibility you may use the cxxtools binary protocol.
It is the fastest protocol supported by cxxtools but you won't find any other
implementation that the cxxtools one.

The logic is similar to rsonrpc raw. It do not use http but just socket
connections. To change the jsonrpc raw server to use binary, we change the
header to `<cxxtools/bin/rpcserver.h>`. The server is now:

        cxxtools::bin::RpcServer server(eventLoop, 7001);

Everything else is kept as is.

The client is again trivial to port. Change the header to
`<cxxtools/bin/rpcclient.h>` and the client is found just in another namespace:

    cxxtools::bin::RpcClient client("", 7001);

Service, namespace, domain
--------------------------

One feature should be mentioned here. XML-RPC and jsonrpc over http both share
the http protocol and hence both have something like a namespace in C++: the
url. You can easily register multiple service objects using different urls on
the same server. You can even mix XML-RPC and jsonrpc services in one server.
That is not possible in jsonrpc raw and binary protocol.

When developing applications it is sometimes useful to group functions into
services and register them separately. Therefore jsonrpc raw and binary protocol
have both features, which make them more similar.

### jsonrpc raw

Jsonrpc raw protocol do not support services in the protocol itself. But since
the name of the function is just a string, you may add a prefix into the
function name. That is directly supported by the jsonrpc raw server and client
of cxxtools.

Instead of registering functions directly to the json rpc server, you may
use a class of type `cxxtools::ServiceRegistry` and register all functions from
that registry in the json rpc server with a prefix. This is done like that:

    cxxtools::json::RpcServer server(eventLoop, 7001);
    cxxtools::ServiceRegistry service;
    service.registerFunction("add", myAdd);
    server.addService("calc.", service);

Now the server knows the function "calc.add". Note that we add a '.' at the end
of the prefix to use it as a separator between prefix and function name.

On client side you may either use:

    cxxtools::json::RpcClient client("", 7001);
    cxxtools::RemoteProcedure<double, double, double> add(client, "calc.add");

or use the prefix method of the json rpcclient, which automatically adds the
prefix to the function name:

    cxxtools::json::RpcClient client("", 7001);
    client.prefix("calc.");
    cxxtools::RemoteProcedure<double, double, double> add(client, "add");

The advantage of this is that the remote procedure class is defined exactly like
in the two http based protocols.

### binary

The binary protocol is quite similar to jsonrpc raw but has built in support for
a `domain`. To register the functions using a domain we use again the
ServiceRegistry class:

    cxxtools::bin::RpcServer server(eventLoop, 7001);
    cxxtools::ServiceRegistry service;
    service.registerFunction("add", myAdd);
    server.addService("calc", service);

This time we do not need a separator since the domain and function name are
already seprate entities in the protocol itself.

The client has a additional parameter in the constructor, which is used to
specify the domain:

    cxxtools::bin::RpcClient client("", 7001, "calc");

Parallel rpc calls
------------------

Sometimes rpc calls to a remote server may take a while. The client just waits
for the result. It may well be, that the client needs results from multiple rpc
calls before it can continue say from the same or different servers. Since the
server runs in parallel it may execute multiple rpc calls at the same time. But
our client sends one request and waits for the result before sending possibly
the next request. It is even more true, if the requests are sent to different
servers.

One solution is to start separate threads on the client side and execute the
calls in parallel. The application needs to implement some synchronization to
know, when all requests are finished. Not really hard to implement but also a
little more than trivial. Not what we are used so far regarding rpc with
cxxtools.

Of course there is a better solution for this in cxxtools. Otherwise I wouldn't
have mentioned it. We can run multiple requests in parallel. For that we need 2
clients and a `cxxtools::Selector`, which monitors the I/O activity needed to
execute the requests and receive the results. Lets look at a example:

    cxxtools::Selector selector;

    cxxtools::xmlrpc::HttpClient client1(selector, "", 7001, "/xmlrpc");
    cxxtools::xmlrpc::HttpClient client2(selector, "", 7001, "/xmlrpc");

    cxxtools::RemoteProcedure<double, double, double> add1(client1, "add");
    cxxtools::RemoteProcedure<double, double, double> add2(client2, "add");

    add1.begin(4, 6);
    add2.begin(5, 7);

    std::cout << "4 + 6 = " << add1.end() << std::endl;
    std::cout << "5 + 7 = " << add2.end() << std::endl;

What happens here? We have 2 clients, which is instantiated with the same
selector. The selector is needed for asynchronous calls, which is actually done
here. For each client we instantiate a separate remote procedure call object.
Instead of using the `operator()`, which executes a synchronous rpc call we use
the `begin` method, which takes the same parameters but do not yet execute the
call and do not return anything yet. It tells procedure, which rpc call to
execute.

Using the `end` method of the `RemoteProcedure`, we fetch the result. But when
the rpc call is actually executed? It is the `end` method which executes the I/O
events until the remote procedure is finished. Since both procedures share the
same selector, the call to `end()` of `add1` executes I/O events for `add2`
also. Bot calls are initiated in parallel. The `end()` method of `add1` blocks
until `add1` has received a result and returns it. When the `end()` method of
`add2` is called, the execution has already started. It may even be finished if
it was faster than `add1`. Again the `end()` method blocks just until `add2` is
finished, which may be immediately the case.

This works with all 4 protocols.

Reconnect, keep alive, threading
--------------------------------

A few words must be said about the technical details just to know, what to
expect.

The client connects to the server as soon as a request is executed and keeps the
connection after execution. When a second call with the same client is done, it
reuses the connection. If the connection is lost in the meanwhile since e.g. the
server was restarted, the client reconnects automatically.

The client can be shared between multiple threads but one client can't execute
multiple requests in parallel. The user has to ensure that e.g. by using a
suitable mutex lock.

The `cxxtools::RemoteProcedure` collects the result of the call. It is kept as
long as the object lives. If you have longer running applications where multiple
remote calls are executed, the best performance is, when the client is kept as
long as possible but the remote procedure is defined locally, so that memory is
released as soon as possible.

On the server side requests are executed in parallel. The server maintains a
dynamic thread pool and each request is executed in a separate thread. It may
well be, that one function is called in several threads at the same time when
multiple requests are received in parallel.

The number of of threads can be set using the methods `minThreads(number)` and
`maxThreads(number)` for each server. By default the minimum number of threads
is 5 and the maximum 200.
