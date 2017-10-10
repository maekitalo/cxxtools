/*
 * Copyright (C) 2017 Tommi Maekitalo
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

/*
 *  This is a tcp server, which demonstrate non blocking I/O on stdin.
 *
 *  The class cxxtools::Cin is a cxxtools::IStream, which is a std::istream
 *  with non blocking I/O.
 *  The server accepts connections on a port (default is 1234, can be changed
 *  with the command line optin -p). It reads from stdin and sends everything
 *  to all connected clients.
 *
 *  To try it out, start the server in one terminal and execute e.g. netcat,
 *  which is often installed, on one or more other terminals using
 *  `nc localhost 1234`. Everything you enter now on the keyboard to `insrv`
 *  is echoed to the nc processes.
 *
 */

#include <iostream>
#include <cxxtools/application.h>
#include <cxxtools/stdstream.h>
#include <cxxtools/net/tcpserver.h>
#include <cxxtools/net/tcpstream.h>
#include <cxxtools/arg.h>
#include <cxxtools/log.h>
#include <vector>

log_define("insrv")

class Insrv : public cxxtools::Application
{
    cxxtools::Cin in;
    cxxtools::net::TcpServer srv;

    typedef std::vector<cxxtools::net::TcpStream*> Clients;
    Clients _clients;

    void onInput(cxxtools::IStream& s);
    void onOutput(cxxtools::net::TcpStream& s);
    void onConnect(cxxtools::net::TcpServer& srv);
    void onClosed(cxxtools::net::TcpStream& s);

public:
    Insrv(int& argc, char* argv[]);
    ~Insrv();
};

Insrv::Insrv(int& argc, char* argv[])
    : cxxtools::Application(argc, argv)
{
    cxxtools::Arg<std::string> ip(argc, argv, 'i');
    cxxtools::Arg<unsigned short> port(argc, argv, 'p', 1234);

    // Attach our stdin device to the event loop of the application
    // and inform that we want to read from there.

    in.setSelector(loop());
    in.beginRead();
    cxxtools::connect(in.inputReady, *this, &Insrv::onInput);

    // Initialize our tcp server class.
    srv.listen(ip, port);
    srv.setSelector(loop());
    cxxtools::connect(srv.connectionPending, *this, &Insrv::onConnect);
}

Insrv::~Insrv()
{
    for (Clients::iterator it = _clients.begin(); it != _clients.end(); ++it)
        delete *it;
}

void Insrv::onInput(cxxtools::IStream& s)
{
    try
    {
        // Finalize reading, which was started with beginRead.
        // This does the actual read and fills the input buffer.
        s.endRead();
    }
    catch (const std::exception& e)
    {
        log_info("read failed with error: " << e.what());
        loop().exit();
        return;
    }

    log_debug("onInput; in_avail=" << s.in_avail());

    if (!s.good())
    {
        log_debug("exit loop");
        loop().exit();
        return;
    }

    char buffer[8192];
    while (s.in_avail())
    {
        unsigned count = s.readsome(buffer, sizeof(buffer));

        // For each client we put the data to the output buffer and initiate writing.
        for (Clients::iterator it = _clients.begin(); it != _clients.end(); ++it)
        {
            log_debug("write " << count << " bytes to client " << static_cast<void*>(*it));
            (*it)->write(buffer, count);
            (*it)->beginWrite();
        }
    }

    log_debug("read");
    s.beginRead();
}

void Insrv::onOutput(cxxtools::net::TcpStream& s)
{
    try
    {
        // This signals that the client is ready to receive data without blocking.
        // We finish the write operation and if there is more to send, we initiate
        // the next write operation, so that we are signalled, when the device
        // is ready to receive more data.

        log_trace("endWrite " << static_cast<void*>(&s));
        s.endWrite();
        if (s.out_avail() > 0)
        {
            log_debug("beginWrite " << static_cast<void*>(&s));
            s.beginWrite();
        }
    }
    catch (const std::exception& e)
    {
        onClosed(s);
    }
}

void Insrv::onClosed(cxxtools::net::TcpStream& s)
{
    log_debug("client disconnected " << static_cast<void*>(&s));
    for (Clients::iterator it = _clients.begin(); it != _clients.end(); ++it)
    {
        if ((*it) == &s)
        {
            log_debug("remove client " << static_cast<void*>(*it));
            delete *it;
            _clients.erase(it);
            break;
        }
    }
}

void Insrv::onConnect(cxxtools::net::TcpServer& srv)
{
    // A new client is detected. We create a new client class
    // and connect the needed signals.

    // Close is either signaled with a exception when a I/O operation is
    // finished using `endRead` or `endWrite` or if a close is detected in the
    // event loop while no I/O operation is pending the signal `closed` is
    // sent.

    cxxtools::net::TcpStream* cli = new cxxtools::net::TcpStream(srv);
    log_debug("new client connected " << static_cast<void*>(cli));
    _clients.push_back(cli);
    cli->setSelector(loop());
    cxxtools::connect(cli->outputReady, *this, &Insrv::onOutput);
    cxxtools::connect(cli->closed, *this, &Insrv::onClosed);
}

int main(int argc, char* argv[])
{
    try
    {
        log_init();
        Insrv app(argc, argv);
        app.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}
