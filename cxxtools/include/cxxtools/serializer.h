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
#ifndef cxxtools_Serializer_h
#define cxxtools_Serializer_h

#include <cxxtools/api.h>
#include <cxxtools/convert.h>
#include <cxxtools/serializationinfo.h>
#include <cxxtools/noncopyable.h>
#include <vector>
#include <map>

namespace cxxtools {

class Formatter;
class SerializationContext;

class CXXTOOLS_API ISerializer
{
    public:
        virtual ~ISerializer()
        {}

        virtual void fixdown(SerializationContext& context) = 0;

        virtual void setName(const std::string& name) = 0;

        virtual void setId(const std::string& id) = 0;

        virtual void format(Formatter& formatter) = 0;

    protected:
        ISerializer()
        {}

        static void fixdownEach(cxxtools::SerializationInfo& si, SerializationContext& context);

        static void formatEach(const cxxtools::SerializationInfo& si, Formatter& formatter);
};


template <typename T>
class Serializer : public ISerializer
{
    public:
        Serializer()
        : _current(&_si)
        { }

        void begin(const T& type)
        {
            _si.clear();
            _si <<= type;
        }

        virtual void fixdown(SerializationContext& context)
        {
            this->fixdownEach( _si, context );
        }

        virtual void setId(const std::string& id)
        {
            _si.setId(id);
        }

        virtual void setName(const std::string& name)
        {
            _si.setName(name);
        }

        virtual void format(Formatter& formatter)
        {
            this->formatEach( _si, formatter );
        }

    private:
        SerializationInfo _si;
        SerializationInfo* _current;
};


class CXXTOOLS_API SerializationContext : private NonCopyable
{
    public:
        SerializationContext();

        virtual ~SerializationContext();

        void clear();

        template <typename T>
        ISerializer* begin(const T& type)
        {
            Serializer<T>* serializer = new Serializer<T>;
            serializer->begin(type);
            this->do_begin(&type, serializer);
            return serializer;
        }

        //TODO:
        // void begin(ISerializer& serializer);

        ISerializer* find(const void* p) const;

        void fixdown(Formatter& formatter);

    private:
        void do_begin(const void* target, ISerializer* serializer);

        std::map<const void*, ISerializer*> _omap;
        std::vector<ISerializer*> _stack;
};

} // namespace cxxtools

#endif
