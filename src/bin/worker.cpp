/*
 * Copyright (C) 2011 Tommi Maekitalo
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

#include "worker.h"
#include "rpcserverimpl.h"
#include <cxxtools/log.h>
#include "socket.h"

log_define("cxxtools.bin.worker")

namespace cxxtools
{
namespace bin
{

void Worker::run()
{
    log_info("new thread running");
    while (!_server.isTerminating() && _server._queue.numWaiting() < _server.minThreads())
    {
        Socket* socket = _server._queue.get();

        if (_server.isTerminating())
        {
            log_debug("server is terminating - quit thread");
            _server._queue.put(socket);
            break;
        }

        if (_server._queue.numWaiting() == 0)
            _server.noWaitingThreads();

        try
        {
            if (!socket->hasAccepted())
            {
                try
                {
                    // do blocking accept
                    socket->accept();
                    log_debug("connection accepted from " << socket->getPeerAddr());

                    if (_server.isTerminating())
                    {
                        log_debug("server is terminating - quit thread");
                        _server._queue.put(socket);
                        break;
                    }

                    // new connection arrived - create new accept socket
                    log_info("new connection accepted from " << socket->getPeerAddr());
                    _server._queue.put(new Socket(*socket));
                }
                catch (const std::exception&)
                {
                    _server._queue.put(new Socket(*socket));
                    throw;
                }
            }
            else if (socket->isConnected())
            {
                log_debug("process available input from " << socket->getPeerAddr());
                socket->onInput(socket->buffer());
            }
            else
            {
                log_debug("socket is not connected any more; delete " << static_cast<void*>(socket));
                log_info("client " << socket->getPeerAddr() << " closed connection");
                delete socket;
                continue;
            }

            Connection inputConnection = connect(socket->buffer().inputReady,
                socket->inputSlot);

            while (socket->wait(10) && socket->isConnected())
                ;

            if (socket->isConnected())
            {
                log_debug("timeout processing socket");
                inputConnection.close();
                _server.addIdleSocket(socket);
            }
            else if (_server.isTerminating())
            {
                _server._queue.put(socket);
            }
            else
            {
                log_debug("socket is not connected any more; delete " << static_cast<void*>(socket));
                log_info("client " << socket->getPeerAddr() << " closed connection");
                delete socket;
            }
        }
        catch (const std::exception& e)
        {
            log_debug("error occured in device: " << e.what() << "; delete " << static_cast<void*>(socket));
            delete socket;
        }
    }

    log_info("thread terminated");
    _server.threadTerminated(this);
}


}
}
