Handling connections in cxxtools    {#connection}
================================

There are several classes, which implements higher level protocols on top of
TCP/IP. They are:

 * `cxxtools::http::Client`
 * `cxxtools::xmlrpc::HttpClient`
 * `cxxtools::json::RpcClient`
 * `cxxtools::json::HttpClient`
 * `cxxtools::bin::RpcClient`

All classes handle the connection to the server the same way.

To connect to a server all need at least a IP address and a port. The classes
all can be either default constructed or initialized with the constructor. When
default constructed the user have to call the `connect` method before they are
usable. For each `connect` variant there is a constructor, which calls `connect`
on initialization.

By default the `connect` method do not actually do a network connection. Hence
the method never fails (except on memory overflow). The method just stores the
connection parameters for later use.

When a actual request is to be done, the class checks, whether it has already a
network connection. When not, it connects to the remote site before sending the
actual request. If the connection fails, the request throws an exception.

The connection is kept open until the class is destroyed or the `connect` method
is called to change the connection parameters.

If it has already a connection, it just uses it. If the request fails, it tries
to reconnect once.

All `connect` methods have a additional parameter of type `bool` named
`realConnect`. It defaults to `false` but when explicitly set to `true`, the
`connect` method already connects to the server. If the connection fails, a
exception is thrown. That is a good way to check, if the server is available.

Note that the new connection parameters are still stored so to make the client
usable you have to either change the connection parameters by calling the
`connect` method or just fix the problem by e.g. starting the server.
