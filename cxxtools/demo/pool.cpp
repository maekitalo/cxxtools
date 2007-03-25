/* pool.cpp - demo for class cxxtools::Pool
 *
 * cxxtools - general purpose C++-toolbox
 * Copyright (C) 2007 Tommi Maekitalo
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include <iostream>
#include <cxxtools/thread.h>
#include <cxxtools/pool.h>
#include <cxxtools/log.h>
#include <cxxtools/loginit.h>

log_define("pooldemo");

// define a dummy dbconnection-object
class Connection
{
    std::string db;
  public:
    Connection()
    {
      log_info("create connection");
    }

    Connection(const std::string& db_)
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
      sleep(1);
    }
};

// define a custom connector for passing the connection string
class Connector
{
    std::string db;

  public:
    Connector(const std::string& db_)
      : db(db_)
      { }
    Connection* operator() ()
      { return new Connection(db); }
};

// define a connection-pool-type
typedef cxxtools::Pool<Connection, Connector> ConnectionPoolType;

// define a thread, which fetches a connection from a pool
// and does something
class MyThread : public cxxtools::AttachedThread
{
    ConnectionPoolType& pool;
    unsigned threadNum;
    unsigned sec;

  public:
    explicit MyThread (ConnectionPoolType& pool_, unsigned threadNum_, unsigned sec_ = 1)
      : pool(pool_),
        threadNum(threadNum_),
        sec(sec_)
      { }

    void run()
    {
      log_info("start thread " << threadNum);

      sleep(sec);

      // We fetch a object from the pool, and call a method pool.get() does
      // not return a connection, but a proxy object, so we have to take
      // care not to assign the object to a Connection, but use that proxy
      // directy.
      //
      // This would be wrong:
      //   Connection conn = *pool.get(); // convert the proxy-object
      //   conn.doSomething(threadNum);  // the connection is back in the pool here :-(
      //
      // The reason is, that the proxy object is destroyed too early.
      // The proxy object puts the connection back to the free-list of the
      // pool, before we use the connection.
      //
      log_info("doSomething in thread " << threadNum);
      pool.get()->doSomething();
      log_info("doSomething ends in thread " << threadNum);

      log_info("thread ready " << threadNum);
    }
};

int main(int argc, char* argv[])
{
  try
  {
    log_init_info();

    ConnectionPoolType connectionPool(3, Connector("mydb"));

    MyThread th1(connectionPool, 1, 2);
    MyThread th2(connectionPool, 2);
    MyThread th3(connectionPool, 3, 3);
    MyThread th4(connectionPool, 4, 3);
    MyThread th5(connectionPool, 5, 4);

    th1.create();
    th2.create();
    th3.create();
    th4.create();
    th5.create();

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

