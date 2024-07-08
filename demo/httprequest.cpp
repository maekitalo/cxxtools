/*
 * Copyright (C) 2003,2009 Tommi Maekitalo
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

#include <cxxtools/http/client.h>
#include <cxxtools/http/request.h>
#include <cxxtools/arg.h>
#include <cxxtools/eventloop.h>
#include <cxxtools/timer.h>
#include <cxxtools/log.h>

#include <exception>
#include <iostream>
#include <thread>

class AsyncRequester : public cxxtools::Connectable
{
        cxxtools::http::Client& _client;
        char** _argv;
        cxxtools::Timespan _wait;

        cxxtools::http::Request _request;
        cxxtools::EventLoop _loop;
        cxxtools::Timer _timer;

        std::size_t onBodyAvailable(cxxtools::http::Client& client);
        void onReady(cxxtools::http::Client& client);
        void onTimeout();

    public:
        AsyncRequester(cxxtools::http::Client& client, char** argv, cxxtools::Timespan wait)
            : _client(client),
                _argv(argv),
                _wait(wait)
        {
            connect(_client.bodyAvailable, *this, &AsyncRequester::onBodyAvailable);
            connect(_client.replyFinished, *this, &AsyncRequester::onReady);
            connect(_timer.timeout, *this, &AsyncRequester::onTimeout);
            _client.setSelector(_loop);
            _loop.add(_timer);
        }

        void run()
        {
            if (*_argv)
            {
                _request.url(*_argv++);
                _client.beginExecute(_request);
                _loop.run();
            }
        }
};

std::size_t AsyncRequester::onBodyAvailable(cxxtools::http::Client& client)
{
    char buffer[8192];
    std::streamsize n = client.in().readsome(buffer, sizeof(buffer));
    std::cout.write(buffer, n);

    return n;
}

void AsyncRequester::onReady(cxxtools::http::Client& client)
{
    if (*_argv)
    {
        if (_wait > cxxtools::Timespan(0))
            _timer.start(_wait);
        else
        {
            _request.url(*_argv++);
            client.beginExecute(_request);
        }
    }
    else
    {
        _loop.exit();
    }

}

void AsyncRequester::onTimeout()
{
    _timer.stop();
    _request.url(*_argv++);
    _client.beginExecute(_request);
}

int main(int argc, char* argv[])
{
    try
    {
        log_init(argc, argv);

        cxxtools::Arg<std::string> user(argc, argv, 'u'); // passed as "username:password"
        cxxtools::Arg<std::string> server(argc, argv, 's');
        cxxtools::Arg<unsigned short int> port(argc, argv, 'p', 80);
        cxxtools::Arg<bool> async(argc, argv, 'a');
        cxxtools::Arg<cxxtools::Seconds> wait(argc, argv, 'w'); // wait between requests in seconds

        cxxtools::http::Client client(server, port);

        if (user.isSet())
        {
            std::string::size_type p = user.getValue().find(':');
            if (p != std::string::npos)
            {
                client.auth(user.getValue().substr(0, p),
                                        user.getValue().substr(p + 1));
            }
            else
            {
                client.auth(user, std::string());
            }
        }

        if (async)
        {
            AsyncRequester ar(client, argv + 1, wait);
            ar.run();
        }
        else
        {
            for (int a = 1; a < argc; ++a)
            {
                if (wait.getValue() > cxxtools::Timespan(0) && a > 1)
                    std::this_thread::sleep_for(std::chrono::milliseconds(cxxtools::Milliseconds(wait)));

                std::cout << client.get(argv[a]).body();
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "ERROR: " << e.what() << std::endl;
    }
}
