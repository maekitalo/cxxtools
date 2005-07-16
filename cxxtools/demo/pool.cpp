/* pool.cpp
   Copyright (C) 2003-2005 Tommi Maekitalo

This file is part of cxxtools.

Cxxtools is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Cxxtools is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Cxxtools; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330,
Boston, MA  02111-1307  USA
*/

#include <exception>
#include <iostream>
#include <cxxtools/pool.h>

// define a dummy dbconnection-object
class dummy_dbconnection
{
  public:
    dummy_dbconnection()
    {
      std::cout << "create connection" << std::endl;
    }
    dummy_dbconnection(const std::string& db)
    {
      std::cout << "create connection to " << db << std::endl;
    }
    ~dummy_dbconnection()
    {
      std::cout << "destroy connection" << std::endl;
    }
};

// if we need to pass parameters to the constructor,
// we define a functor, which creates objects
class connector
{
    std::string db;
  public:
    connector(const std::string& db_)
      : db(db_)
      { }
    dummy_dbconnection* operator() ()
      { return new dummy_dbconnection(db); }
};

// just a dummy-function to show, that we can use the connection
void do_something(dummy_dbconnection&)
{
  std::cout << "use connection" << std::endl;
}

int main(int argc, char* argv[])
{
  try
  {
    {
      // define a connectionpool with default creator
      cxxtools::Pool<dummy_dbconnection> connectionpool;

      do_something(connectionpool.get());
      do_something(connectionpool.get());
      do_something(connectionpool.get());
      std::cout << "drop all free connections" << std::endl;
      connectionpool.drop();
      do_something(connectionpool.get());
      do_something(connectionpool.get());
    }

    {
      // define a connectionpool with custom creator
      cxxtools::Pool<dummy_dbconnection, connector> mydb_connectionpool(connector("mydb"));
      do_something(mydb_connectionpool.get());
      do_something(mydb_connectionpool.get());
      do_something(mydb_connectionpool.get());
      std::cout << "drop all free connections" << std::endl;
      mydb_connectionpool.drop();
      do_something(mydb_connectionpool.get());
      do_something(mydb_connectionpool.get());
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

