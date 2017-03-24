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

#include <iostream>
#include <cxxtools/application.h>
#include <cxxtools/iostream.h>
#include <cxxtools/stddevice.h>
#include <cxxtools/net/tcpserver.h>
#include <cxxtools/net/tcpstream.h>
#include <cxxtools/arg.h>
#include <cxxtools/log.h>
#include <vector>

log_define("insrv")

class Insrv : public cxxtools::Application
{
    cxxtools::StdinDevice indev;
    cxxtools::IOStream in;
    cxxtools::net::TcpServer srv;

    typedef std::vector<cxxtools::net::TcpStream*> Clients;
    Clients _clients;

    void onInput(cxxtools::StreamBuffer& sb);
    void onOutput(cxxtools::StreamBuffer& cli);
    void onConnect(cxxtools::net::TcpServer& srv);

public:
    Insrv(int& argc, char* argv[]);
    ~Insrv();
};

Insrv::Insrv(int& argc, char* argv[])
    : cxxtools::Application(argc, argv)
{
    cxxtools::Arg<std::string> ip(argc, argv, 'i');
    cxxtools::Arg<unsigned short> port(argc, argv, 'p', 1234);

    indev.setSelector(&loop());
    in.attachDevice(indev);
    in.buffer().beginRead();
    cxxtools::connect(in.buffer().inputReady, *this, &Insrv::onInput);

    srv.listen(ip, port);
    srv.setSelector(&loop());
    cxxtools::connect(srv.connectionPending, *this, &Insrv::onConnect);
}

Insrv::~Insrv()
{
    for (Clients::iterator it = _clients.begin(); it != _clients.end(); ++it)
        delete *it;
}

void Insrv::onInput(cxxtools::StreamBuffer& sb)
{
    try
    {
        sb.endRead();
    }
    catch (const std::exception& e)
    {
        log_info("read failed with error: " << e.what());
        loop().exit();
        return;
    }

    log_debug("onInput; in_avail=" << sb.in_avail());

    if (sb.in_avail() == 0 || sb.device()->eof())
    {
        log_debug("exit loop");
        loop().exit();
        return;
    }

    char buffer[8192];
    while (sb.in_avail())
    {
        unsigned count = std::min(
            static_cast<unsigned>(sb.in_avail()),
            static_cast<unsigned>(sizeof(buffer)));

        count = sb.sgetn(buffer, count);

        for (Clients::iterator it = _clients.begin(); it != _clients.end(); ++it)
        {
            log_debug("write " << count << " bytes to client " << static_cast<void*>(*it));
            (*it)->write(buffer, count);
            (*it)->buffer().beginWrite();
        }
    }

    log_debug("read");
    sb.beginRead();
}

void Insrv::onOutput(cxxtools::StreamBuffer& sb)
{
    try
    {
        log_debug("endWrite " << static_cast<void*>(&sb));
        sb.endWrite();
        if (sb.out_avail() > 0)
        {
            log_debug("beginWrite " << static_cast<void*>(&sb));
            sb.beginWrite();
        }
    }
    catch (const std::exception& e)
    {
        log_debug("client disconnected");
        for (Clients::iterator it = _clients.begin(); it != _clients.end(); ++it)
        {
            if ((*it)->rdbuf() == &sb)
            {
                log_debug("remove client");
                _clients.erase(it);
                break;
            }
        }
    }
}

void Insrv::onConnect(cxxtools::net::TcpServer& srv)
{
    log_debug("new client connected");

    cxxtools::net::TcpStream* cli = new cxxtools::net::TcpStream(srv);
    _clients.push_back(cli);
    cli->buffer().device()->setSelector(&loop());
    cxxtools::connect(cli->buffer().outputReady, *this, &Insrv::onOutput);
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
