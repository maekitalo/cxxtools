/*
 * Copyright (C) 2008 by Marc Boris Duerner
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
#ifndef cxxtools_Decomposer_h
#define cxxtools_Decomposer_h

#include <cxxtools/serializationinfo.h>
#include <string>

namespace cxxtools
{

class Formatter;

class IDecomposer
{
    public:
        typedef SerializationInfo::int_type int_type;
        typedef SerializationInfo::unsigned_type unsigned_type;

        virtual ~IDecomposer()
        {}

        virtual void setName(const std::string& name) = 0;

        virtual void format(Formatter& formatter) = 0;

        static void formatEach(const SerializationInfo& si, Formatter& formatter);

};


template <typename T>
class Decomposer : public IDecomposer
{
    public:
        Decomposer()
        : _current(&_si)
        { }

        void begin(const T& type)
        {
            _si.clear();
            _si <<= type;
        }

        virtual void setName(const std::string& name)
        {
            _si.setName(name);
        }

        virtual void format(Formatter& formatter)
        {
            formatEach( _si, formatter );
        }

    private:
        SerializationInfo _si;
        SerializationInfo* _current;
};


template <>
class Decomposer<SerializationInfo> : public IDecomposer
{
    public:
        Decomposer()
        : _current(&_si)
        { }

        void begin(const SerializationInfo& type)
        {
            _si.clear();
            _si <<= type;
        }

        void begin(SerializationInfo& type)
        {
            _si.clear();
            _si.swap(type);
        }

        void begin(SerializationInfo&& type)
        {
            _si = std::move(type);
        }

        virtual void setName(const std::string& name)
        {
            _si.setName(name);
        }

        virtual void format(Formatter& formatter)
        {
            formatEach( _si, formatter );
        }

    private:
        SerializationInfo _si;
        SerializationInfo* _current;
};


} // namespace cxxtools

#endif
