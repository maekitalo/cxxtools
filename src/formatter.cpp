/*
 * Copyright (C) 2011 by Tommi Maekitalo
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

#include <cxxtools/formatter.h>
#include <cxxtools/convert.h>

namespace cxxtools
{
void Formatter::addValueStdString(const std::string& name, const std::string& type,
                         std::string&& value)
{
    addValueString(name, type, String::widen(value));
}

void Formatter::addValueChar(const std::string& name, const std::string& type,
                         char value)
{
    addValueString(name, type, String(1, Char(value)));
}

void Formatter::addValueBool(const std::string& name, const std::string& type,
                         bool value)
{
    addValueString(name, type, convert<String>(value));
}

void Formatter::addValueInt(const std::string& name, const std::string& type,
                         int_type value)
{
    addValueString(name, type, convert<String>(value));
}

void Formatter::addValueUnsigned(const std::string& name, const std::string& type,
                         unsigned_type value)
{
    addValueString(name, type, convert<String>(value));
}

void Formatter::addValueFloat(const std::string& name, const std::string& type,
                         float value)
{
    addValueString(name, type, convert<String>(value));
}

void Formatter::addValueDouble(const std::string& name, const std::string& type,
                         double value)
{
    addValueString(name, type, convert<String>(value));
}

void Formatter::addValueLongDouble(const std::string& name, const std::string& type,
                         long double value)
{
    addValueString(name, type, convert<String>(value));
}

void Formatter::addNull(const std::string& name, const std::string& type)
{
    addValueString(name, type, String());
}

}

