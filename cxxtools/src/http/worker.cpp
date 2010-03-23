/*
 * Copyright (C) 2010 Tommi Maekitalo
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
#include "serverimpl.h"
#include <cxxtools/log.h>
#include "socket.h"

log_define("cxxtools.http.worker")

namespace cxxtools
{
namespace http
{

void Worker::run()
{
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
                _server._queue.put(new Socket(*socket));
            }
            else if (socket->isConnected())
            {
                log_debug("process available input");
                socket->onInput(socket->buffer());
            }
            else
            {
                log_debug("socket is not connected any more; delete " << static_cast<void*>(socket));
                delete socket;
            }

            socket->setSelector(&_selector);
            connect(socket->buffer().inputReady, socket->inputSlot);

            while (_selector.wait(_server.idleTimeout()) && socket->isConnected())
                ;

            if (socket->isConnected())
            {
                log_debug("timeout processing socket");
                socket->setSelector(0);
                disconnect(socket->buffer().inputReady, socket->inputSlot);
                _server.addIdleSocket(socket);
            }
            else if (_server.isTerminating())
            {
                _server._queue.put(socket);
            }
            else
            {
                log_debug("socket is not connected any more; delete " << static_cast<void*>(socket));
                delete socket;
            }
        }
        catch (const std::exception& e)
        {
            log_debug("error occured in device: " << e.what() << "; delete " << static_cast<void*>(socket));
            delete socket;
        }
    }

    log_debug("thread terminated");
    _server.threadTerminated(this);
}


}
}
