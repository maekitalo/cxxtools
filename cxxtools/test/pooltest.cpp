////////////////////////////////////////////////////////////////////////
// pooltest.cpp
//

#include "cxxtools/pool.h"
#include "cxxtools/thread.h"
#include <iostream>

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
