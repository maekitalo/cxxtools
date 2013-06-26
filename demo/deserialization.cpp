/*
 * Copyright (C) 2013 Tommi Maekitalo
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * is provided AS IS, WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, and
 * NON-INFRINGEMENT.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 *
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

