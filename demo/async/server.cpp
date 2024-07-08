#include <cxxtools/arg.h>
#include <cxxtools/eventloop.h>
#include <cxxtools/log.h>
#include <cxxtools/net/tcpserver.h>
#include <cxxtools/net/tcpstream.h>

#include <algorithm>
#include <iostream>
#include <vector>

log_define("server")

class Responder;

////////////////////////////////////////////////////////////////////////
// Define a server class
//
class Server : public cxxtools::Connectable
{
    cxxtools::net::TcpServer _server;
    std::vector<Responder*> _responder;

    void onNewConnection(cxxtools::net::TcpServer&);
    void onClosed(cxxtools::net::TcpStream&);

public:
    explicit Server(cxxtools::SelectorBase* selector, const std::string& ip, unsigned short port);
};

Server::Server(cxxtools::SelectorBase* selector, const std::string& ip, unsigned short port)
    : _server(ip, port)
{
    _server.setSelector(selector);
    // When a connection arrives from a client the signal 'connectionPending' is emmitted.
    // We connect that to our handler.
    cxxtools::connect(_server.connectionPending, *this, &Server::onNewConnection);
}

////////////////////////////////////////////////////////////////////////
// The Responder class handles a single client.
//
class Responder : public cxxtools::net::TcpStream
{
    void onInput(cxxtools::net::TcpStream&);
    void onOutput(cxxtools::net::TcpStream&);

public:
    explicit Responder(cxxtools::net::TcpServer& server);
};

////////////////////////////////////////////////////////////////////////

void Server::onNewConnection(cxxtools::net::TcpServer&)
{
    // When a new connection arrives create a new client responder.

    log_debug("new connection");
    Responder* responder = new Responder(_server);
    _responder.push_back(responder);

    // When the client closes the connection the signal 'closed' is emmitted.
    // Connect that to our handler onClosed.
    cxxtools::connect(responder->closed, *this, &Server::onClosed);
}

void Server::onClosed(cxxtools::net::TcpStream& responder)
{
    // When a client is closed we delete the Responder
    std::vector<Responder*>::iterator it = std::find(_responder.begin(), _responder.end(), &responder);
    if (it != _responder.end())  // should always be true
    {
        log_debug("closed");
        _responder.erase(it);
        delete *it;
    }
    else
    {
        log_warn("responder not found");
    }
}

////////////////////////////////////////////////////////////////////////
Responder::Responder(cxxtools::net::TcpServer& server)
    : cxxtools::net::TcpStream(server)
{
    setSelector(server.selector());

    // connect the signals 'inputReady' and 'outputReady' to our handlers.
    cxxtools::connect(inputReady, *this, &Responder::onInput);
    cxxtools::connect(outputReady, *this, &Responder::onOutput);

    // Start waiting for input. When data arrives, the signal 'inputReady' is
    // emmitted.
    beginRead();
}

void Responder::onInput(cxxtools::net::TcpStream&)
{
    log_debug("onInput");

    // Data has arrived. End read reads data from the socket and fills our
    // input buffer.
    endRead();

    if (in_avail() == 0)
    {
        // When 'inputReady' is emmitted without data the client has actually
        // closed the connection. We emmit the signal 'closed'.
        closed(*this);
        return;
    }

    // echo input back to the client
    char ch;
    while (get(ch), *this)
    {
        // Put the character to the output buffer
        put(ch);
    }

    // We wrote at least one character to the output buffer.
    // We signal, that we want to send the data as soon as the socket is
    // ready to accept outgoing data without blocking.
    beginWrite();

    // We called endRead. Now we want to continue reading and put the device
    // back into read mode.
    beginRead();
}

void Responder::onOutput(cxxtools::net::TcpStream&)
{
    log_debug("out avail=" << buffer().out_avail());

    // The device has signaled, that it is ready to accept data to be sent.
    // The call to endWrite copies data from our output buffer to the socket.
    endWrite();

    // The socket may have accepted only part of our output buffer so there
    // may be data left. We tell the device, that we want to transfer more
    // data when there is space in the output buffer.
    if (buffer().out_avail())
        beginWrite();
}

////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
    try
    {
        // read options from command line
        // -l <ip>   specifies the ip address of the interface to listen on
        //           a empty string tells cxxtools to listen on all interfaces.
        // -p <port> specifies to port with a default value of 7000.
        cxxtools::Arg<std::string> ip(argc, argv, 'l');
        cxxtools::Arg<unsigned short> port(argc, argv, 'p', 7000);

        // This initializes logging by reading a file `server.properties` or
        // `server.json` in the local directory.
        log_init(argc, argv);

        // We need a event loop, which handles I/O events
        cxxtools::EventLoop loop;

        // Our server runs in the loop.
        Server server(&loop, ip, port);

        // Run the loop
        loop.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

