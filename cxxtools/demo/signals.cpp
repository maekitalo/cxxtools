/*
 * Copyright (C) 2008 Marc Boris Duerner
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
#include <cxxtools/signal.h>
#include <iostream>

int function()
{
  std::cout << "function called." << std::endl;
  return 0;
}

struct Object : public cxxtools::Connectable
{
  bool method()
  {
    std::cout << "Object::method called." << std::endl;
    return true;
  }

  char constMethod() const
  {
    std::cout << "Object::constMethod called." << std::endl;
    return 'r';
  }

  static std::string staticMethod()
  {
    std::cout << "Object::staticMethod called." << std::endl;
    return "test";
  }
};


int main(int argc, char* argv[])
{
  try
  {
    Object obj;
    cxxtools::Signal<> signal;
    signal.connect( cxxtools::slot(function) );
    signal.connect( cxxtools::slot(obj, &Object::method) );
    signal.connect( cxxtools::slot(obj, &Object::constMethod) );
    signal.connect( cxxtools::slot(&Object::staticMethod) );

    signal.send();
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
    return 1;
  }
}
