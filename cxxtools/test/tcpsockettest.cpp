/*
 * Copyright (C) 2006 - 2009 by Marc Boris Duerner
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

#include "cxxtools/unit/assertion.h"
#include "cxxtools/unit/testsuite.h"
#include "cxxtools/unit/registertest.h"
#include "cxxtools/net/tcpserver.h"
#include "cxxtools/net/tcpsocket.h"
#include <string>

class TcpSocketTest : public cxxtools::unit::TestSuite
{
    public:
        TcpSocketTest()
        : cxxtools::unit::TestSuite("TcpSocketTest")
        {
            this->registerMethod( "NonBlockingWithSelector", *this,
                                  &TcpSocketTest::NonBlockingWithSelector);
            this->registerMethod( "NonBlockingWithWait", *this,
                                  &TcpSocketTest::NonBlockingWithWait);
            this->registerMethod( "ConnectFailed", *this,
                                  &TcpSocketTest::ConnectFailed);
        }

        void setUp()
        {
            _acceptor = new cxxtools::net::TcpSocket();
        }

        void tearDown()
        {
            delete _acceptor;
        }

        void ConnectFailed()
        {
            cxxtools::Selector selector;

            cxxtools::net::TcpSocket client;
            connect(client.connected, *this, &TcpSocketTest::onConnectFailed);

            client.beginConnect("", 9000);
            selector.add(client);
            selector.wait(1000);
        }

        void onConnectFailed(cxxtools::net::TcpSocket& socket)
        {
            CXXTOOLS_UNIT_ASSERT_THROW(socket.endConnect(), cxxtools::IOError);
        }

        void NonBlockingWithWait()
        {
            this->reportMessage("\nSTART");

            cxxtools::net::TcpServer server("", 8000);
            connect(server.connectionPending, *this, &TcpSocketTest::onAccept);

            connect(_acceptor->inputReady, *this, &TcpSocketTest::onInput);

            cxxtools::net::TcpSocket client;
            client.beginConnect("", 8000);
            connect(client.connected, *this, &TcpSocketTest::onConnect);
            connect(client.outputReady, *this, &TcpSocketTest::onOutput);

            server.wait(1000);
            client.wait(1000);
            _acceptor->wait(1000);
            this->reportMessage("FINISHED");
        }

        void NonBlockingWithSelector()
        {
            this->reportMessage("\nSTART");
            cxxtools::Selector selector;

            cxxtools::net::TcpServer server("", 8000);
            connect(server.connectionPending, *this, &TcpSocketTest::onAccept);
            selector.add(server);

            connect(_acceptor->inputReady, *this, &TcpSocketTest::onInput);
            selector.add(*_acceptor);

            cxxtools::net::TcpSocket client;
            client.beginConnect("", 8000);
            connect(client.connected, *this, &TcpSocketTest::onConnect);
            connect(client.outputReady, *this, &TcpSocketTest::onOutput);
            selector.add(client);

            selector.wait(1000);
            selector.wait(1000);
            selector.wait(1000);
            selector.wait(1000);

            this->reportMessage("FINISHED");
        }

        void onAccept(cxxtools::net::TcpServer& server)
        {
            _acceptor->accept(server);
            this->reportMessage( "ACCEPTED IP: " + _acceptor->getSockAddr() +
                                 " PEER: " + _acceptor->getPeerAddr() );

            _acceptor->beginRead(input, 200);
        }

        void onConnect(cxxtools::net::TcpSocket& socket)
        {
            socket.endConnect();
            this->reportMessage( "CONNECTED IP: " + socket.getSockAddr() +
                                 " PEER: " + socket.getPeerAddr() );

            static const char buffer[] = "Hello World !!!";
            socket.beginWrite(buffer, sizeof(buffer));
        }

        void onInput(cxxtools::IODevice& device)
        {
            std::size_t n = device.endRead();
            std::string msg("INPUT RECEIVED: ");
            msg.append(input, n);
            this->reportMessage(msg);
            CXXTOOLS_UNIT_ASSERT(n > 5);
        }

        void onOutput(cxxtools::IODevice& device)
        {
            std::size_t n = device.endWrite();
            this->reportMessage("OUTPUT SENT");
            CXXTOOLS_UNIT_ASSERT(n > 5);
        }

    private:
        cxxtools::net::TcpSocket* _acceptor;
        char input[200];

};

cxxtools::unit::RegisterTest<TcpSocketTest> register_TcpSocketTest;
