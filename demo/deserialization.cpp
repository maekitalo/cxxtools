/*
 * Copyright (C) 2013 Tommi Maekitalo
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

#include <iostream>
#include <cxxtools/arg.h>
#include <cxxtools/json.h>
#include <cxxtools/csv.h>
#include <cxxtools/xml/xml.h>

class Object
{
    friend void operator>>= (const cxxtools::SerializationInfo& si, Object& object);

    std::string _stringValue;
    int _intValue;

  public:
    Object()
      : _intValue(0)
      { }

    Object(const std::string& stringValue, int intValue)
      : _stringValue(stringValue),
        _intValue(intValue)
      { }

    const std::string& stringValue() const
    { return _stringValue; }

    int intValue() const
    { return _intValue; }
};

void operator<<= (cxxtools::SerializationInfo& si, const Object& object)
{
  si.setTypeName("Object");
  si.addMember("stringValue") <<= object.stringValue();
  si.addMember("intValue") <<= object.intValue();
}

void operator>>= (const cxxtools::SerializationInfo& si, Object& object)
{
  si.getMember("stringValue") >>= object._stringValue;
  si.getMember("intValue") >>= object._intValue;
}

int main(int argc, char* argv[])
{
  try
  {
    cxxtools::Arg<bool> xml(argc, argv, 'x');
    cxxtools::Arg<bool> csv(argc, argv, 'c');

    std::vector<Object> o;

    if (xml)
      std::cin >> cxxtools::xml::Xml(o);
    else if (csv)
      std::cin >> cxxtools::Csv(o);
    else // json
      std::cin >> cxxtools::Json(o);

    for (unsigned n = 0; n < o.size(); ++n)
    {
      std::cout << "o[" << n << "].stringValue = " << o[n].stringValue() << '\n'
                << "o[" << n << "].intValue    = " << o[n].intValue()    << '\n';
    }
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

