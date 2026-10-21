Handling connections in cxxtools
================================

There are several classes that implement higher-level protocols on top of
TCP/IP:

 * cxxtools::http::Client
 * cxxtools::xmlrpc::HttpClient
 * cxxtools::json::RpcClient
 * cxxtools::json::HttpClient
 * cxxtools::bin::RpcClient

All these classes handle connections to the server in the same way.

To connect to a server, they need at least an IP address and a port. The
classes can either be default-constructed or initialized using a constructor
that takes the connection parameters. When a class is default-constructed,
the user has to call the connect method before it can be used. For each
connect variant, there is also a corresponding constructor that calls connect
during initialization.

By default, the connect method does not actually establish a network
connection. Therefore, the method does not fail except in case of a memory
allocation failure. It only stores the connection parameters for later use.

When an actual request is made, the class checks whether a network connection
has already been established. If not, it connects to the remote server before
sending the request. If the connection fails, the request throws an exception.

The connection is kept open until the class is destroyed or the connect method
is called to change the connection parameters.

If a connection has already been established, it is reused. If a request
fails, the class tries to reconnect once.

All connect methods have an additional parameter of type bool named
realConnect. It defaults to false. When explicitly set to true, the connect
method immediately establishes a connection to the server. If the connection
fails, an exception is thrown. This provides a convenient way to check whether
the server is available.

Note that the new connection parameters are still stored even if establishing
the connection fails. To make the client usable, you therefore have to either
change the connection parameters by calling the connect method again or fix
the underlying problem, for example by starting the server.
