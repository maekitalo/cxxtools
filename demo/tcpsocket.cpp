/**
    This is a very minimalistic and somewhat incomplete demo
    how to communicate using tcp sockets in cxxtools

    It has 3 modes of operation - a client, a server for a single connection
    and a server using a event loop.

    In both server modes it waits for a incoming connection, reads
    a single message to a fixed buffer and outputs it.

    In client mode it connects to a server and writes a messsage to the socket.

    To run it, you need to terminals. One for the server and one for the client.

    Run in one terminal `./tcpsocket -l` or `./tcpsocket -L` and in the
    other `./tcpsocket`. The first terminal will print the greeting "Hello",
    which was sent by the client.
 */

#include <cxxtools/net/tcpsocket.h>
#include <cxxtools/net/tcpserver.h>
#include <cxxtools/eventloop.h>
#include <cxxtools/arg.h>
#include <cxxtools/log.h>
#include <iostream>

cxxtools::EventLoop eventLoop;
char buffer[8192];

void onInput(cxxtools::IODevice& dev)
{
    std::cout << "onInput" << std::endl;
    auto count = dev.endRead();
    std::cout.write(buffer, count);
    delete &dev;
    eventLoop.exit();
}

int main(int argc, char* argv[])
{
    log_init();

    try
    {
        cxxtools::Arg<std::string> ip(argc, argv, 'i');
        cxxtools::Arg<unsigned short> port(argc, argv, 'p', 1234);

        cxxtools::Arg<bool> listen(argc, argv, 'l');
        cxxtools::Arg<bool> loop(argc, argv, 'L');

        if (loop)
        {
            cxxtools::net::TcpServer server(ip, port);
            auto socket = new cxxtools::net::TcpSocket(server);
            std::cout << "connected" << std::endl;
            socket->setSelector(&eventLoop);
            socket->beginRead(buffer, sizeof(buffer));
            cxxtools::connect(socket->inputReady, onInput);
            eventLoop.run();
        }
        else if (listen)
        {
            cxxtools::net::TcpServer server(ip, port);
            cxxtools::net::TcpSocket socket(server);

            auto count = socket.read(buffer, sizeof(buffer));
            std::cout.write(buffer, count);
        }
        else
        {
            cxxtools::net::TcpSocket socket(ip, port);
            std::string msg = "Hallo\n";
            socket.write(msg.data(), msg.size());
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

