/*
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
////////////////////////////////////////////////////////////////////////
// pooltest.cpp
//

#include "cxxtools/pool.h"
#include "cxxtools/thread.h"
#include <iostream>
#include <unistd.h>

Mutex logmutex;

#undef LOG
#define LOG(expr)  do { MutexLock lock(logmutex); std::cout << expr << std::endl; } while(0)

////////////////////////////////////////////////////////////////////////
// Connection
//
class Connection
{
    static unsigned n;
    unsigned m_n;

  public:
    Connection(const char* i);
    ~Connection()
    { LOG("~Connection()"); }

    void doSomething(unsigned thread, unsigned sleep_time);
};

unsigned Connection::n = 0;

Connection::Connection(const char* i)
  : m_n(n++)
{
  LOG("Connection::Connection() " << m_n);
}

void Connection::doSomething(unsigned thread, unsigned sleep_time)
{
  LOG("Connection::doSomething() " << thread << '/' << m_n);
  sleep(sleep_time);
  LOG(" <= Connection::doSomething() " << thread << '/' << m_n);
}

////////////////////////////////////////////////////////////////////////
// connectionpool
//
class connector
{
    const char* connectionstring;
  public:
    connector(const char* cs)
      : connectionstring(cs)
    { }

    Connection* operator() ()
    { return new Connection(connectionstring); }
};

typedef pool<Connection, connector> connectionpool;

////////////////////////////////////////////////////////////////////////
// myThread
//
class myThread : public Thread
{
    connectionpool& m_pool;
    unsigned num;

  public:
    myThread(unsigned n, connectionpool& p)
      : m_pool(p),
        num(n)
    { }
    void Run();
};

void myThread::Run()
{
  LOG("run thread " << num);
  for (int i = 0; i < 2; ++i)
  {
    {
      connectionpool::objectptr_type po2;
      {
        connectionpool::objectptr_type po = m_pool.get();
        po->doSomething(num, 1);

        po2 = po;
      }

      Connection& conn(po2);
      conn.doSomething(num, 1);
    }
    sleep(1);
  }
}

////////////////////////////////////////////////////////////////////////
// main
//
int main(int argc, char* argv[])
{
  const unsigned numThreads = 2;
  const unsigned numConn = 2;
  try
  {
    connectionpool connections(numConn, connector("uhu"));
    myThread* t[numThreads];

    for (unsigned i = 0; i < numThreads; ++i)
    {
      t[i] = new myThread(i, connections);
      t[i]->Create();
    }

    for (unsigned i = 0; i < numThreads; ++i)
      t[i]->Join();

    LOG("******************************");

    connections.drop(1);

    for (unsigned i = 0; i < numThreads; ++i)
      t[i]->Create();

    for (unsigned i = 0; i < numThreads; ++i)
    {
      t[i]->Join();
      delete t[i];
    }

    LOG("main ends");
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}
