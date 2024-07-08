/*
 * Copyright (C) 2007 Tommi Maekitalo
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

#include <cxxtools/pool.h>
#include <cxxtools/timespan.h>
#include <cxxtools/log.h>

#include <thread>
#include <iostream>
#include <functional>

#include <unistd.h>

log_define("pooldemo")

// define a dummy dbconnection-object
class Connection
{
        std::string db;
    public:
        Connection()
        {
            log_info("create connection");
        }

        explicit Connection(const std::string& db_)
            : db(db_)
        {
            log_info("create connection to \"" << db << '"');
        }

        ~Connection()
        {
            log_info("destroy connection");
        }

        void doSomething()
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
};

// define a custom connector for passing the connection string
class Connector
{
        std::string db;

    public:
        explicit Connector(const std::string& db_)
            : db(db_)
            { }
        Connection* operator() ()
            { return new Connection(db); }
};

// define a connection-pool-type
typedef cxxtools::Pool<Connection, Connector> ConnectionPoolType;

// define a thread, which fetches a connection from a pool
// and does something
class MyThread
{
        std::thread _thread;
        ConnectionPoolType& _pool;
        unsigned _threadNum;
        cxxtools::Seconds _sec;

    public:
        explicit MyThread (ConnectionPoolType& pool_, unsigned threadNum_, cxxtools::Seconds sec = 1)
            : _thread(std::bind(&MyThread::run, this)),
                _pool(pool_),
                _threadNum(threadNum_),
                _sec(sec)
            { }

            void join()
            { _thread.join(); }

        void run()
        {
            log_info("start thread " << _threadNum);

            std::this_thread::sleep_for(std::chrono::milliseconds(cxxtools::Milliseconds(_sec)));

            // We fetch a object from the pool, and call a method pool.get() does
            // not return a connection, but a proxy object, so we have to take
            // care not to assign the object to a Connection, but use that proxy
            // directy.
            //
            // This would be wrong:
            //     Connection conn = *pool.get(); // convert the proxy-object
            //     conn.doSomething(threadNum);    // the connection is back in the pool here :-(
            //
            // The reason is, that the proxy object is destroyed too early.
            // The proxy object puts the connection back to the free-list of the
            // pool, before we use the connection.
            //
            log_info("doSomething in thread " << _threadNum);
            _pool.get()->doSomething();
            log_info("doSomething ends in thread " << _threadNum);

            log_info("thread ready " << _threadNum);
        }
};

int main(int argc, char* argv[])
{
    try
    {
        log_init(argc, argv);

        ConnectionPoolType connectionPool(3, Connector("mydb"));

        MyThread th1(connectionPool, 1, 2);
        MyThread th2(connectionPool, 2);
        MyThread th3(connectionPool, 3, 3);
        MyThread th4(connectionPool, 4, 3);
        MyThread th5(connectionPool, 5, 4);

        log_info("threads created");

        th1.join();

        // Thread 1 is ready and the connection is put back into the pool.
        // We release all free connections here. At least one connection
        // from thread 1 is released.
        log_info("pool drop");
        connectionPool.drop();

        th2.join();
        th4.join();
        th3.join();
        th5.join();

        log_info("threads joined");
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}
