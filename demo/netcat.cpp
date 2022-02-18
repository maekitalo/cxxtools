/*
 * Copyright (C) 2003 Tommi Maekitalo
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

#include <cxxtools/net/tcpstream.h>
#include <cxxtools/sslctx.h>
#include <cxxtools/arg.h>
#include <cxxtools/log.h>

#include <exception>
#include <iostream>
#include <fstream>

int main(int argc, char* argv[])
{
    try
    {
        log_init();

        cxxtools::Arg<std::string> ip(argc, argv, 'i');
        cxxtools::Arg<unsigned short> port(argc, argv, 'p', 1234);
        cxxtools::Arg<unsigned> bufsize(argc, argv, 'b', 8192);
        cxxtools::Arg<bool> listen(argc, argv, 'l');
        cxxtools::Arg<bool> read_reply(argc, argv, 'r');
        cxxtools::Arg<bool> ssl(argc, argv, 's');
        cxxtools::Arg<std::string> cert(argc, argv, "--cert");
        cxxtools::Arg<std::string> ca(argc, argv, "--CA");

        if (listen)
        {
            // I'm a server

            // listen to a port
            cxxtools::net::TcpServer server(ip.getValue(), port);

            // accept a connetion
            cxxtools::net::TcpStream worker(server, bufsize);

            if (ssl)
            {
                cxxtools::SslCtx sslCtx = sslCtx.standard();

                if (cert.isSet())
                    sslCtx.loadCertificateFile(cert);
                if (ca.isSet())
                    sslCtx.setVerify(2, ca);
                worker.sslAccept(sslCtx);
            }

            // copy to stdout
            std::cout << worker.rdbuf();
        }
        else
        {
            // I'm a client

            // connect to server
            cxxtools::net::TcpStream peer(ip, port, bufsize);

            if (ssl)
            {
                cxxtools::SslCtx sslCtx = sslCtx.standard();
                if (cert.isSet())
                    sslCtx.loadCertificateFile(cert);
                peer.sslConnect(sslCtx);
            }

            if (argc > 1)
            {
                for (int a = 1; a < argc; ++a)
                {
                    std::ifstream in(argv[a]);
                    peer << in.rdbuf() << std::flush;
                }
            }
            else
            {
                // copy stdin to server
                peer << std::cin.rdbuf() << std::flush;
            }

            if (read_reply)
                // copy answer to stdout
                std::cout << peer.rdbuf() << std::flush;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}
