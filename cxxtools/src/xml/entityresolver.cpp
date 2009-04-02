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
#include "cxxtools/xml/entityresolver.h"
#include <sstream>
#include <iostream>

namespace cxxtools {

namespace xml {

EntityResolver::EntityResolver()
{
    this->clear();
}


EntityResolver::~EntityResolver()
{
}


void EntityResolver::clear()
{
    _entityMap.clear();

    EntityMap::value_type lt(L"lt", L"<");
    _entityMap.insert(lt);

    EntityMap::value_type gt(L"gt", L">");
    _entityMap.insert(gt);

    EntityMap::value_type amp(L"amp", L"&");
    _entityMap.insert(amp);

    EntityMap::value_type apos(L"apos", L"\'");
    _entityMap.insert(apos);

    EntityMap::value_type qout(L"quot", L"\"");
    _entityMap.insert(qout);
}


void EntityResolver::addEntity(const String& entity, const String& token)
{
  _entityMap.insert( std::make_pair<String, String>(entity, token) );
}


String EntityResolver::resolveEntity(const String& entity)
{
    if( entity.find('#') == 0 )
    {
        int code;
        std::stringstream ss1;
        if( entity.find('x') == 1 )
        {
            ss1 << entity.substr(2).narrow();
            ss1 << std::hex;
        }
        else
        {
            ss1 << entity.substr(1).narrow();
            String number;
        }
        ss1 >> code;
        return String( 1, Char(code) );
    }

    std::map<String, String>::iterator it = _entityMap.find(entity);
    if( it == _entityMap.end() )
    {
        return String();
    }

    return it->second;
}

} // namespace xml

} // namespace cxxtools
