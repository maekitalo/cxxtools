RPC with cxxtools
=================

Introduction
------------

In larger systems, we often need to use functionality provided by a different
process, either on the same host or over a network on another host. We may need
data from another process or want to trigger an action there. For this, we need
a connection to the other process and a protocol that specifies how requests
and replies are interpreted.

cxxtools provides an easy-to-use and flexible RPC framework for this purpose.
The framework provides classes for both the server and client sides.

It is not limited to a single RPC protocol but currently supports four
protocols:

- XML-RPC
- JSON-RPC over HTTP
- raw JSON-RPC
- cxxtools binary RPC

All of them share a common cxxtools API, making it easy to start with one
protocol and switch to another later.

Let's start with one protocol and come back to the others later. We start with
XML-RPC because it is widely standardized and was also the first protocol
supported by cxxtools.

XML-RPC server
-------------

We start by implementing an XML-RPC server. Implementing a server is normally
quite difficult because we want to handle multiple clients in parallel without
one client having to wait for another to complete. cxxtools makes this almost
trivial.

Let's define some functions that we want to call remotely:

```cpp
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
```

Since XML-RPC uses HTTP, we need an HTTP server. This is implemented by the
class `cxxtools::http::Server`. It requires an event loop in which I/O events
and other events are processed.

The important lines in the `main` function are:

```cpp
    int main(int argc, char* argv[])
    {
        try
        {
            log_init(argc, argv);

            cxxtools::EventLoop eventLoop;
            cxxtools::http::Server server(eventLoop, 7001);
            eventLoop.run();
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }
```

We pass the event loop and the port on which the server should accept
connections to the constructor. By default, the server listens on all local
interfaces. It is also possible to listen on one specific local IP address or
on multiple interfaces. The IP addresses may be IPv4 or IPv6 addresses. In our
example, we simply use the default.

The call to `log_init()` is optional. I usually add it. It looks for a file
named `log.xml` in the current directory and initializes logging.

With the proper includes, our HTTP server is already ready to run. However, we
have not yet connected the HTTP server to our functions. We first have to
define the URL used to call them.

We also need a way to call the functions. With XML-RPC, we receive a function
name and parameters encoded as XML. Our XML-RPC server has to parse the XML,
extract the function name and parameters, and call the corresponding function.
The return value then has to be converted back to XML and sent as an HTTP
reply.

That sounds like a lot of work. Fortunately, cxxtools does all of this for us.
First, we define a service object:

```cpp
    cxxtools::xmlrpc::Service service;
```

We register our functions with the service:

```cpp
    service.registerFunction("add", myAdd);
    service.registerFunction("sub", mySub);
    service.registerFunction("mul", myMul);
    service.registerFunction("div", myDiv);
```

Then we connect the service to the server:

```cpp
    server.addService("/xmlrpc", service);
```

All of this is done before starting the event loop. That is all we need. Put
the pieces together, compile the server, and we have a working XML-RPC server
with four functions.

Here is the complete code:

```cpp
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
            log_init(argc, argv);

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
```

Store the code in `xmlrpcserver.cpp` and compile and link it.

So where did all the XML parsing and formatting go? It is handled by
`cxxtools::xmlrpc::Service` and the `registerFunction()` method.
`registerFunction()` is a template function that generates the code needed to
extract the parameters and format the return value.

XML-RPC client
--------------

To test our XML-RPC server, we need a client. The client has to connect to the
server, create an XML document containing a request, send the XML over HTTP,
receive the reply, and interpret it.

Again, cxxtools provides helper classes that make this easy. First, we need an
XML-RPC client:

```cpp
    cxxtools::xmlrpc::HttpClient client("", 7001, "/xmlrpc");
```

The client constructor takes the IP address, port, and URL as parameters. We
pass an empty string as the IP address, which tells it to connect to the local
server. Depending on the configuration of the machine, IPv4 or IPv6 is used.
Both are supported by our server.

Next, we define a remote procedure object, which handles the formatting for us.
cxxtools provides the template class `cxxtools::RemoteProcedure` with a
variable number of template parameters. The first template parameter specifies
the return type, while the remaining parameters specify the types of the
arguments.

The constructor takes the client and the function name:

```cpp
    cxxtools::RemoteProcedure<double, double, double> add(client, "add");
```

We are now ready to call our remote function. The `RemoteProcedure` object
provides an `operator()` that takes the specified arguments. To add the numbers
12 and 17 on the server, we simply write:

```cpp
    double result = add(12, 17);
```

To put everything together, we need the appropriate includes and a `main`
function. Here is the complete client including all four functions:

```cpp
    #include <iostream>
    #include <cxxtools/xmlrpc/httpclient.h>
    #include <cxxtools/remoteprocedure.h>
    #include <cxxtools/log.h>

    int main(int argc, char* argv[])
    {
        try
        {
            log_init(argc, argv);

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
```

Store the code in a file named `xmlrpcclient.cpp` and compile it.

We are now ready to run both programs. Start a second terminal, run the server
in one terminal, and run the client in the other.

The client should print the results of the calculations.

It can also be helpful to enable logging. Create a basic logging configuration
and run the server again. You will then see log output, including the requests
received from the client.

XML-RPC error handling
----------------------

Looking at our server code, we can see that the function `myDiv`, which is
exported to the client as `div`, throws an exception when the divisor is zero.

Try changing the client so that it passes zero as the divisor. The exception is
then received by the client. cxxtools makes this handling transparent. The main
difference is that the original exception type is lost. On the client side, the
exception is always a `cxxtools::RemoteException`, which is derived from
`std::exception`. Only the exception text is preserved.

Looking at the definition of `cxxtools::RemoteException`, we can see that it
also supports an optional return code. This code is preserved as well.
Therefore, if a `cxxtools::RemoteException` with an error code is thrown on the
server side, the error code is available on the client side.

Note that exceptions are also thrown for technical errors, for example when the
client cannot connect to the server.

Serialization
-------------

RPC has some inherent limitations. XML-RPC can transfer data, but it cannot
transfer arbitrary objects such as file descriptors or window handles. Let's
therefore look at the possible parameter types.

According to the XML-RPC data model, scalars, structs, and arrays can be
transferred. The XML-RPC implementation in cxxtools therefore needs a way to
convert C++ data to XML and back. This process is called serialization and
deserialization.

There is a separate document that describes serialization in detail, so we
will concentrate on the minimum required here.

cxxtools provides a data structure called `cxxtools::SerializationInfo`. This
class stores the information needed to generate XML, and incoming XML is
converted back into this structure. To use our own parameter types, we have to
tell cxxtools how to convert them to `SerializationInfo` and back.

This is already implemented for all built-in types, including standard library
containers. That is why the examples above work without any additional
serialization code.

For custom data types, we need to define two operators. Let's look at an
example:

```cpp
    struct MyStruct
    {
        std::string stringValue;
        int intValue;
        std::vector<double> doubleValues;
    };
```

We want to use this structure as a parameter or return type of an XML-RPC
function. We therefore add two operators:

```cpp
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
```

The first operator defines how a `MyStruct` is converted into a
`cxxtools::SerializationInfo`. The second operator converts it back. That is
all we need to do.

All members of the struct are standard types, so cxxtools already knows how to
handle them.

As always in C++, we have to avoid duplicate symbols. The operators are
normally either declared in the header and defined in a `.cpp` file, or, if
they are fully defined in a header, marked as `inline`.

We can now define a function such as:

```cpp
    MyStruct getMyStruct()
    {
        MyStruct s;
        s.stringValue = "Hi";
        s.intValue = 17;
        s.doubleValues.push_back(17);
        s.doubleValues.push_back(42);
        return s;
    }
```

and make it available as an XML-RPC service just like the `myAdd`, `mySub`,
`myMul`, and `myDiv` functions. We do not show the complete example here
because it would duplicate most of the server code above and can easily be
derived from that example.

Of course, both client and server have to know the structure we defined.

Other protocols
---------------

As mentioned earlier, cxxtools is not limited to XML-RPC. XML-RPC is a good
choice because it is simple and flexible. Its main disadvantage is its
verbosity. The other three protocols are faster.

Let's look at the other protocols and how they are used with cxxtools.

### JSON-RPC over HTTP

The protocol most similar to XML-RPC is JSON-RPC over HTTP because it uses the
same HTTP transport.

Only a few lines need to be changed to make our server use JSON-RPC over HTTP.
The first is the include. Instead of `<cxxtools/xmlrpc/service.h>`, we include
`<cxxtools/json/httpservice.h>`.

The second change is the service object. Instead of
`cxxtools::xmlrpc::Service`, we use `cxxtools::json::HttpService`. For clarity,
we also change the URL from `/xmlrpc` to `/jsonrpc`.

Here is the complete server using JSON-RPC over HTTP:

```cpp
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
            log_init(argc, argv);

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
```

Only three lines differ from the XML-RPC server. To build the server, store the
code in `jsonhttpserver.cpp` and compile and link it.

The server needs to be linked against `cxxtools-json` instead of
`cxxtools-xmlrpc`.

Since both protocols share the same HTTP server but use different URLs, they
can even be combined in a single server. Simply define both services and add
both to the HTTP server.

Before we can test our JSON-RPC-over-HTTP server, we still need a client.
Changing the client is just as easy as changing the server. In fact, only two
lines need to be changed.

Replace `<cxxtools/xmlrpc/httpclient.h>` with
`<cxxtools/json/httpclient.h>` and define the client as:

```cpp
    cxxtools::json::HttpClient client("", 7001, "/jsonrpc");
```

That is all we need to do. We do not show the complete example here because
the remaining code is unchanged.

### JSON-RPC raw

The next supported protocol is raw JSON-RPC. It does not use HTTP as its
transport protocol but communicates directly over a socket connection, making
it even more compact.

This time, we do not instantiate an HTTP server but a JSON-RPC server. We also
do not need a separate service object and instead register our functions
directly with the server.

The server is defined in `<cxxtools/json/rpcserver.h>`.

We do not show the complete code because it is again almost identical to the
previous examples. Here is just the `main` function of the raw JSON-RPC server:

```cpp
    int main(int argc, char* argv[])
    {
        try
        {
            log_init(argc, argv);

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
```

There is not much new here. It is even simpler because there is no separate
service object.

The client is equally simple to change. It is defined in
`<cxxtools/json/rpcclient.h>`:

```cpp
    cxxtools::json::RpcClient client("", 7001);
```

Note that there is no service URL.

### cxxtools binary RPC

If compatibility with other implementations is not required, you can use the
cxxtools binary RPC. It is the fastest protocol supported by cxxtools, but
it is specific to cxxtools.

The protocol is more compact and faster than JSON-RPC or XML-RPC. It also
preserves floating-point values without converting them to a decimal
representation and back, as JSON-RPC and XML-RPC do.

The logic is similar to raw JSON-RPC. It does not use HTTP, but communicates
directly over socket connections.

To change the raw JSON-RPC server to use the cxxtools binary RPC, change the
header to `<cxxtools/bin/rpcserver.h>`. The server is then created as follows:

```cpp
    cxxtools::bin::RpcServer server(eventLoop, 7001);
```

Everything else remains unchanged.

Adapting the client is just as easy. Change the header to
`<cxxtools/bin/rpcclient.h>` and use the client from the `cxxtools::bin`
namespace:

```cpp
    cxxtools::bin::RpcClient client("", 7001);
```

Service, namespace, domain
--------------------------

There is one more feature worth mentioning. XML-RPC and JSON-RPC over HTTP
both use HTTP and therefore have something similar to a namespace: the URL.
You can register multiple service objects using different URLs on the same
server. You can even mix XML-RPC and JSON-RPC services in one server.

Raw JSON-RPC and the cxxtools binary RPC do not provide this through HTTP URLs.

When developing applications, however, it is often useful to group functions
into services and register them separately. Raw JSON-RPC and the cxxtools
binary RPC both provide mechanisms for this.

### JSON-RPC raw

The raw JSON-RPC protocol does not support services directly. However, since
the function name is simply a string, a prefix can be added to the function
name. This is directly supported by the raw JSON-RPC server and client in
cxxtools.

Instead of registering functions directly with the JSON-RPC server, we can use
a `cxxtools::ServiceRegistry` and register all functions from that registry
with a prefix:

```cpp
    cxxtools::json::RpcServer server(eventLoop, 7001);

    cxxtools::ServiceRegistry service;
    service.registerFunction("add", myAdd);

    server.addService("calc.", service);
```

The server now knows the function `calc.add`. Note that we append a `.` to the
prefix to use it as a separator between the prefix and function name.

On the client side, we can either use:

```cpp
    cxxtools::json::RpcClient client("", 7001);
    cxxtools::RemoteProcedure<double, double, double> add(client, "calc.add");
```

or use the `prefix()` method of the JSON-RPC client, which automatically adds
the prefix to the function name:

```cpp
    cxxtools::json::RpcClient client("", 7001);
    client.prefix("calc.");
    cxxtools::RemoteProcedure<double, double, double> add(client, "add");
```

The advantage is that the remote procedure object is then defined in the same
way as with the two HTTP-based protocols.

### cxxtools binary RPC

The cxxtools binary RPC is similar to raw JSON-RPC but has built-in support for
a domain. To register functions using a domain, we again use the
`ServiceRegistry`:

```cpp
    cxxtools::bin::RpcServer server(eventLoop, 7001);

    cxxtools::ServiceRegistry service;
    service.registerFunction("add", myAdd);

    server.addService("calc", service);
```

This time, no separator is required because the domain and function name are
separate entities in the protocol itself.

The client has an additional constructor parameter for specifying the domain:

```cpp
    cxxtools::bin::RpcClient client("", 7001, "calc");
```

The binary data format can also be used independently to serialize and
deserialize data.

Parallel RPC calls
------------------

Sometimes RPC calls to a remote server take a while. A synchronous client waits
for each result before it continues. However, a client may need the results of
multiple RPC calls before it can continue, whether these calls are sent to the
same server or to different servers.

Since the server handles requests in parallel, multiple RPC calls can be
executed at the same time. A synchronous client, however, normally sends one
request and waits for its result before sending the next one.

One solution is to use separate threads on the client side and execute the
calls in parallel. The application then needs synchronization to determine
when all requests have completed. This is possible, but cxxtools provides a
simpler solution.

We can run multiple requests in parallel using two clients and a
`cxxtools::Selector`, which monitors the I/O activity required to execute the
requests and receive their results. We need the header `<cxxtools/selector.h>`.

Let's look at an example:

```cpp
    cxxtools::Selector selector;

    cxxtools::xmlrpc::HttpClient client1(selector, "", 7001, "/xmlrpc");
    cxxtools::xmlrpc::HttpClient client2(selector, "", 7001, "/xmlrpc");

    cxxtools::RemoteProcedure<double, double, double> add1(client1, "add");
    cxxtools::RemoteProcedure<double, double, double> add2(client2, "add");

    add1.begin(4, 6);
    add2.begin(5, 7);

    std::cout << "4 + 6 = " << add1.end() << std::endl;
    std::cout << "5 + 7 = " << add2.end() << std::endl;
```

What happens here? We have two clients instantiated with the same selector. The
selector is required for the asynchronous calls used here. For each client, we
instantiate a separate remote procedure object.

Instead of using `operator()`, which executes a synchronous RPC call, we use
the `begin()` method. It takes the same parameters but does not return the
result. It specifies which RPC call should be executed.

We retrieve the result using the `end()` method. But when is the RPC call
actually executed?

The `end()` method processes I/O events until the remote procedure has
completed. Since both procedures share the same selector, calling `add1.end()`
also processes I/O events for `add2`. Both calls are therefore initiated in
parallel.

The `end()` method of `add1` blocks until `add1` has received a result and then
returns it. When `add2.end()` is called, execution of `add2` has already
started. It may even have completed if it was faster than `add1`. Again,
`end()` blocks only until `add2` has completed, which may be immediately.

More verbosely this can be written as:

```cpp
    cxxtools::Selector selector;

    cxxtools::xmlrpc::HttpClient client1(selector, "", 7001, "/xmlrpc");
    cxxtools::xmlrpc::HttpClient client2(selector, "", 7001, "/xmlrpc");

    cxxtools::RemoteProcedure<double, double, double> add1(client1, "add");
    cxxtools::RemoteProcedure<double, double, double> add2(client2, "add");

    add1.begin(4, 6);
    add2.begin(5, 7);

    // execute I/O events as long as either call is still active
    while (add1.activeProcedure() != 0 || add2.activeProcedure() != 0)
        selector.wait();

    std::cout << "4 + 6 = " << add1.result() << std::endl;
    std::cout << "5 + 7 = " << add2.result() << std::endl;
```

Note that even if `add1.end()` also processes the I/O events for `add2`, an
exception thrown by the remote add2 procedure is reported when `add2.end()` is
called. I/O errors encountered while processing events may, however, be
reported by `add1.end()`.

This works with all four protocols.

Reconnect, keep alive, threading
--------------------------------

A few technical details are worth mentioning so that you know what to expect.

The client connects to the server when a request is executed and keeps the
connection open afterwards. When another call is made using the same client,
the connection is reused. If the connection is lost in the meantime, for
example because the server was restarted, the client reconnects automatically.

A client can be shared between multiple threads, but a single client cannot
execute multiple requests in parallel. The application has to ensure this, for
example by using a suitable mutex.

`cxxtools::RemoteProcedure` stores the result of the call for as long as the
object exists. In long-running applications that execute many remote calls, it
is therefore best to keep the client alive for as long as possible while
creating remote procedure objects locally, so that their memory is released as
soon as possible.

On the server side, requests are executed in parallel. The server maintains a
dynamic thread pool, and each request is executed in a separate thread. A
function may therefore be called concurrently by several threads when multiple
requests are received at the same time.

The number of threads can be configured using the `minThreads(number)` and
`maxThreads(number)` methods of each server. By default, the minimum number of
threads is 5 and the maximum is 200.
