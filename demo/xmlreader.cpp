/*
 * Copyright (C) 2014 Tommi Maekitalo
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

/*
    This program demonstrates how to read a xml structure with xml attributes.
 */

#include <iostream>
#include <sstream>
#include <cxxtools/xml/xml.h>
#include <cxxtools/serializationinfo.h>

// We define a object, which describes the structure, we read from xml.
class MyObject
{
    friend void operator>>= (const cxxtools::SerializationInfo& si, MyObject& obj);

    std::string _stringValue;
    int _intValue;

  public:

    const std::string& stringValue() const
    { return _stringValue; }

    int intValue() const
    { return _intValue; }
};

// The deserialization helper operator converts the SerializationInfo to our object
void operator>>= (const cxxtools::SerializationInfo& si, MyObject& obj)
{
  si.getMember("stringValue") >>= obj._stringValue;
  si.getMember("intValue") >>= obj._intValue;
}

int main(int argc, char* argv[])
{
  try
  {
    // Xml is read from a std::istream
    std::istringstream in("<object stringValue=\"hello\" intValue=\"42\"/>");

    // We define a object and use the deserialization helper function cxxtools::xml::Xml
    // which makes it easy to use the xml deserializer
    //
    // By default the values are read from sub nodes. Here we want to read the
    // xml attributes.
    MyObject o;
    in >> cxxtools::xml::Xml(o).readAttributes(true);

    // To see the result, we print our object to std::cout.
    std::cout << "string value=" << o.stringValue() << '\n'
              << "int value=" << o.intValue() << std::endl;
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

