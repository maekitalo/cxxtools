/*
 * Copyright (C) 2005-2008 by Dr. Marc Boris Duerner
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
#include <cxxtools/serializationinfo.h>

namespace cxxtools {

SerializationInfo::SerializationInfo()
: _parent(0)
, _category(Value)
, _fixupAddr(0)
, _fixupInfo(0)
{ }


SerializationInfo::SerializationInfo(const SerializationInfo& si)
: _parent(si._parent)
, _category(si._category)
, _name(si._name)
, _type(si._type)
, _id(si._id)
, _fixupAddr(si._fixupAddr)
, _fixupInfo(si._fixupInfo)
, _value(si._value)
, _nodes(si._nodes)
{ }


SerializationInfo& SerializationInfo::operator=(const SerializationInfo& si)
{
    _parent = si._parent;
    _category = si._category;
    _name = si._name;
    _id = si._id;
    _type = si._type;
    _value = si._value;
    _nodes = si._nodes;
    _fixupAddr = si._fixupAddr;
    _fixupInfo = si._fixupInfo;
    return *this;
}


void SerializationInfo::reserve(size_t n)
{
    _nodes.reserve(n);
}


void SerializationInfo::setReference(void* ref)
{
    _value = convert<cxxtools::String>(ref);
    _category = Reference;
}


SerializationInfo& SerializationInfo::addReference(const std::string& name, void* ref)
{
    SerializationInfo& info = this->addMember(name);
    info.setReference(ref);
    return info;
}


void SerializationInfo::getReference(void*& type, const std::type_info& ti) const
{
    _fixupAddr = &type;
    _fixupInfo = &ti;
}


void* SerializationInfo::fixupAddr() const
{
    return _fixupAddr;
}


const std::type_info& SerializationInfo::fixupInfo() const
{
    return *_fixupInfo;
}


const cxxtools::String& SerializationInfo::toString() const
{
    return _value;
}


SerializationInfo& SerializationInfo::addMember(const std::string& name)
{
    ///SerializationInfo* info = new SerializationInfo();
    ///info->setParent(*this);
    ///info->setName(name);
    ///_nodes.push_back( info );
    SerializationInfo info;
    _nodes.push_back( info );
    _nodes.back().setParent(*this);
    _nodes.back().setName(name);

    _category = Object;

    ///return *info;
    return _nodes.back();
}


SerializationInfo::Iterator SerializationInfo::begin()
{
    if(_nodes.size() == 0)
        return 0;

    return &( _nodes[0] );
}


SerializationInfo::Iterator SerializationInfo::end()
{
    if(_nodes.size() == 0)
        return 0;

    return &_nodes[0] + _nodes.size();
}


SerializationInfo::ConstIterator SerializationInfo::begin() const
{
    if(_nodes.size() == 0)
        return 0;

    return &( _nodes[0] );
}


SerializationInfo::ConstIterator SerializationInfo::end() const
{
    if(_nodes.size() == 0)
        return 0;

    return &_nodes[0] + _nodes.size();
}


const SerializationInfo& SerializationInfo::getMember(const std::string& name) const
{
    Nodes::const_iterator it = _nodes.begin();
    for(; it != _nodes.end(); ++it)
    {
        if( it->name() == name )
            return *it;
    }

    throw SerializationError("Missing info for '" + name + "'", CXXTOOLS_SOURCEINFO);
}


const SerializationInfo* SerializationInfo::findMember(const std::string& name) const
{
    Nodes::const_iterator it = _nodes.begin();
    for(; it != _nodes.end(); ++it)
    {
        if( it->name() == name )
            return &(*it);
    }

    return 0;
}


SerializationInfo* SerializationInfo::findMember(const std::string& name)
{
    Nodes::iterator it = _nodes.begin();
    for(; it != _nodes.end(); ++it)
    {
        if( it->name() == name )
            return &(*it);
    }

    return 0;
}

void SerializationInfo::clear()
{
    _category = Void;
    _name = std::string();
    _type = _name;
    _id = _name;
    _fixupAddr = 0;
    _fixupInfo = 0;
    _value.clear();
    _nodes.clear();
}

} // namespace cxxtools
