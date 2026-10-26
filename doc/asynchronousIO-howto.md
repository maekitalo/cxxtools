Asynchronous I/O with cxxtools
=============================

Asynchronous I/O in cxxtools is based on the EventLoop described in
the [Event Loop how-to](eventloop-howto.md).

This document focuses on network communication.

In cxxtools, asynchronous I/O always uses a pair of methods. The first
initiates an action and the second finalizes it. Typically, finalization is
triggered by a signal indicating that the operation can be completed without
blocking.

E.g. a network read is initiated by `beginRead`. When the class runs in the
event loop, the signal `inputReady` indicates that there is data available. The
application must call `endRead` to finalize the read operation.

The begin methods normally do not throw exceptions. All results including
errors are issued by the end methods. Error handling is done by throwing
exceptions.

Basic class
-----------

The most basic class of network communication is `cxxtools::IODevice`, which is
an abstraction of a file descriptor. Most network classes are derived from that.

Note that for all asynchronous IO, the event loop must be passed to the classes
to be able to receive the signals. Typically this is passed either as a
parameter in the constructor or set using the method
`setSelector(cxxtools::Selector*)` or `setSelector(cxxtools::Selector&)`.

Objects participating in asynchronous I/O are represented by
`cxxtools::Selectable` instances. The EventLoop monitors registered
selectables and emits the corresponding signals when events occur.

IP addresses
------------

Before describing TCP communication, we need to understand how IP addresses are
handled in cxxtools. An IP address is passed as a `std::string` to the methods.
It can be an IPv4 address, an IPv6 address or a name, which can be resolved. In
client classes, an empty string refers to localhost. When used by server
classes, an empty string means all local interfaces.

Tcp client
----------

A simple TCP client connection is implemented by `cxxtools::net::TcpSocket` in
the header `<cxxtools/net/tcpsocket.h>`, which is derived from
`cxxtools::IODevice`. To connect to a server, the method `beginConnect` with an
IP address and port or a `cxxtools::AddrInfo`, which encapsulates both values,
must be called. When the signal `connected` is emitted the application
completes the operation by calling `endConnect`. The signal passes a reference
to the `cxxtools::TcpSocket` as a parameter.

To read from the socket the method `cxxtools::IODevice::beginRead(char*,
size_t)` is called, which requests that data be read from the socket. The
signal `cxxtools::IODevice::inputReady` is emitted when data is available on
the socket. A `cxxtools::IODevice::endRead()` finalizes the read. It also
returns the number of bytes actually read.

Writing is done similar to read. `cxxtools::IODevice::beginWrite(const char*,
size_t)` initiates the write. The signal `cxxtools::IODevice::outputReady`
tells the application that at least part of the output buffer has been sent.
The write operation is finalized by calling `endWrite()`.

For SSL the methods beginSslConnect/endSslConnect with the signal sslConnected
and beginSslAccept/endSslAccept with the signal sslAccepted are provided. To
shutdown the SSL layer, beginSslShutdown/endSslShutdown are used together with
the signal `sslClosed`.

Tcp server
----------

A TCP server is implemented in `cxxtools::net::TcpServer`. The most important
method is `listen` with an IP address and a port. It starts listening on one or
more interfaces. When an empty string is passed as an IP address the server
listens on all local interfaces.

When a connection is detected, the signal `connectionPending` is emitted. The
application should then accept the connection by instantiating a
`cxxtools::net::TcpSocket`. The socket has a constructor, which takes a
`cxxtools::net::TcpServer` to accept the connection. Alternatively the method
`cxxtools::net::TcpSocket::accept` can be used.

Other derived classes from `cxxtools::net::TcpSocket` have similar methods.

Buffering
---------

A `cxxtools::net::TcpSocket` operates directly on the underlying byte stream.
Reads and writes therefore work with raw buffers and may complete only
partially.

TCP is a stream-oriented protocol. A single read operation may return only part
of the transmitted data, exactly one logical message, or multiple logical
messages at once. Likewise, a write operation may transfer only part of the
supplied buffer.

Applications therefore often need to maintain input and output buffers and keep
track of how much data has already been received or transmitted.

Managing these buffers correctly can become surprisingly complicated and is a
common source of implementation errors.

To simplify this, cxxtools provides `cxxtools::net::BufferedSocket`, which
manages input and output buffering automatically. Instead of working with raw
byte buffers, applications can process higher-level data units such as lines of
text.

Buffering also improves performance. Sending data in larger chunks
significantly reduces overhead and improves throughput.

The `cxxtools::net::BufferedSocket` is derived from `cxxtools::net::TcpSocket`
and maintains input and output buffers.

### Sending data

For sending it offers the method `put` in various flavours. The most obvious one
takes a pointer and a size. It adds data into the output buffer, but does not
begin writing. The application must call `beginWrite()` to inform the event
loop that it should write the data.

Put may be called whenever needed. Even when a write operation is currently
running. In that case the data is written to a second buffer waiting to be
sent.

In case a write operation fails the signal `outputFailed` is issued. The
buffered socket and the exception object are passed as parameters. The signal
is always emitted in a catch handler. Hence the application may call just
`throw` if it wishes to throw the exception further.

The signal written(BufferedSocket&) is emitted to inform the application that
at least part of the output buffer has been successfully sent.

The signal `outputBufferEmpty(BufferedSocket&)` is used to signal the
application that all data is sent successfully.

`write()` is a convenience function combining `put()` and `beginWrite()`.

Note that in contrast to `TcpSocket`, `beginWrite()` may be called even when a
write operation is running. In that case it is just ignored. That way the
application can call it whenever a message is finished.

### Receiving data

To receive data the method `beginRead()` must be called. The event loop will
call the signal `inputReady(BufferedSocket&)` when data is received. The
application must finish reading using `endRead()` in the callback. Then it can
access the input buffer using `std::vector<char>& inputBuffer()`. It can
process as much data as it wishes or decide to leave part or all data in the
input buffer. If more data is expected `beginRead()` must be issued again.

Suppose a line

```text
    Hello World\n
```

is sent over the connection.

The first inputReady signal may contain only

```text
    Hello W
```

while the second delivers

```text
    orld\n
```

The input buffer allows the application to keep incomplete data until a
complete message is available. When `inputReady` is issued, it may decide not
to consume the first `Hello W` but to keep it in the input buffer. Calling
beginRead() requests additional data. When more data arrives, another
inputReady signal is emitted.
The application will then find the complete line `Hello World\n` in the input
buffer.
