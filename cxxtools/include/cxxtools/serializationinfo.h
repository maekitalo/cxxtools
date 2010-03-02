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
#ifndef cxxtools_SerializationInfo_h
#define cxxtools_SerializationInfo_h

#include <cxxtools/api.h>
#include <cxxtools/string.h>
#include <cxxtools/convert.h>
#include <cxxtools/serializationerror.h>
#include <vector>
#include <set>
#include <map>
#include <list>
#include <deque>
#include <typeinfo>
#include <cxxtools/config.h>

namespace cxxtools {

/** @brief Represents arbitrary types during serialization.
*/
class CXXTOOLS_API SerializationInfo
{
    typedef std::vector<SerializationInfo> Nodes;

    public:
        enum Category {
            Void = 0, Value = 1, Object = 2, Array = 6, Reference = 8
        };

        class Iterator;
        class ConstIterator;

    public:
        SerializationInfo();

        SerializationInfo(const SerializationInfo& si);

        ~SerializationInfo()
        {}

        void reserve(size_t n);

        Category category() const
        {
            return _category;
        }

        void setCategory(Category cat)
        {
            _category = cat;
        }

        SerializationInfo* parent()
        {
            return _parent;
        }

        const SerializationInfo* parent() const
        {
            return _parent;
        }

        const std::string& typeName() const
        {
            return _type;
        }

        void setTypeName(const std::string& type)
        {
            _type = type;
        }

        const std::string& name() const
        {
            return _name;
        }

        void setName(const std::string& name)
        {
            _name = name;
        }

        void setId(const std::string& id)
        {
            _id = id;
        }

        const std::string& id() const
        {
            return _id;
        }

        /** @brief Serialization of weak pointers
        */
        void setReference(void* ref);

        /** @brief Serialization of weak pointers
        */
        SerializationInfo& addReference(const std::string& name, void* ref);

        /** @brief Deserialization of weak pointers
        */
        template <typename T>
        void toReference(T*& type) const
        {
            this->getReference( reinterpret_cast<void*&>(type), typeid(T) );
        }

        /** @brief Deserialization of weak member pointers
        */
        template <typename T>
        void getReference(const std::string& name, T*& type) const
        {
            this->getMember(name).getReference( reinterpret_cast<void*&>(type), typeid(T) );
        }

        void* fixupAddr() const;

        const std::type_info& fixupInfo() const;

        /** @brief Serialization of flat data-types
        */
        template <typename T>
        void setValue(const T& value)
        {
            convert(_value, value);
            _category = Value;
        }

        /** @brief Deserialization of flat data-types
        */
        template <typename T>
        T toValue() const
        {
            return convert<T>(_value);
        }

        /** @brief Deserialization of flat data-types
        */
        template <typename T>
        void toValue(T& value) const
        {
            convert(value, _value);
        }

        /** @brief Deserialization of flat member data-types
        */
        const cxxtools::String& toString() const;

        /** @brief Serialization of flat member data-types
        */
        template <typename T>
        SerializationInfo& addValue(const std::string& name, const T& value)
        {
            SerializationInfo& info = this->addMember(name);
            info.setValue(value);
            return info;
        }

        /** @brief Serialization of member data
        */
        SerializationInfo& addMember(const std::string& name);

        /** @brief Deserialization of member data
        */
        const SerializationInfo& getMember(const std::string& name) const;

        /** @brief Compiler workaround.
            This is needed for some compilers (GCC 3.x) to allow access to
            method 'T getValue(const std::string& name) const' below.
         */
        template <typename T>
        friend T getValue(const std::string& name, SerializationInfo* si);

        /** @brief Deserialization of flat child value types
        */
        template <typename T>
        T getValue(const std::string& name) const
        {
            T value;
            const SerializationInfo& info = this->getMember(name);
            info.toValue(value);
            return value;
        }

        /** @brief Deserialization of flat child value types
        */
        template <typename T>
        void getValue(const std::string& name, T& value) const
        {
            const SerializationInfo& info = this->getMember(name);
            return info.toValue(value);
        }

        /** @brief Find member data by name

            This method returns the data for an object with the name \a name.
            or null if it is not present.
        */
        const SerializationInfo* findMember(const std::string& name) const;

        /** @brief Find member data by name

            This method returns the data for an object with the name \a name.
            or null if it is not present.
        */
        SerializationInfo* findMember(const std::string& name);

        size_t memberCount() const
        {
            return _nodes.size();
        }

        Iterator begin();

        Iterator end();

        ConstIterator begin() const;

        ConstIterator end() const;

        SerializationInfo& operator =(const SerializationInfo& si);

        void clear();

    protected:
        void getReference(void*& type, const std::type_info& ti) const;

        void setParent(SerializationInfo& si)
        { _parent = &si; }

    private:
        SerializationInfo* _parent;
        Category _category;
        std::string _name;
        std::string _type;
        std::string _id;
        mutable void* _fixupAddr; // only refs
        mutable const std::type_info* _fixupInfo; // only refs
        cxxtools::String _value;        // values/refs
        Nodes _nodes;             // objects/arrays
};


class SerializationInfo::Iterator
{
    public:
        Iterator();

        Iterator(const Iterator& other);

        Iterator(SerializationInfo* info);

        Iterator& operator=(const Iterator& other);

        Iterator& operator++();

        SerializationInfo& operator*();

        SerializationInfo* operator->();

        bool operator!=(const Iterator& other) const;

    private:
        SerializationInfo* _info;
};


class SerializationInfo::ConstIterator
{
    public:
        ConstIterator();

        ConstIterator(const ConstIterator& other);

        ConstIterator(const SerializationInfo* info);

        ConstIterator& operator=(const ConstIterator& other);

        ConstIterator& operator++();

        const SerializationInfo& operator*() const;

        const SerializationInfo* operator->() const;

        bool operator!=(const ConstIterator& other) const;

    private:
        const SerializationInfo* _info;
};


inline SerializationInfo::Iterator::Iterator()
: _info(0)
{}


inline SerializationInfo::Iterator::Iterator(const Iterator& other)
: _info(other._info)
{}


inline SerializationInfo::Iterator::Iterator(SerializationInfo* info)
: _info(info)
{}


inline SerializationInfo::Iterator& SerializationInfo::Iterator::operator=(const Iterator& other)
{
    _info = other._info;
    return *this;
}


inline SerializationInfo::Iterator& SerializationInfo::Iterator::operator++()
{
    ++_info;
    return *this;
}


inline SerializationInfo& SerializationInfo::Iterator::operator*()
{
    return *_info;
}


inline SerializationInfo* SerializationInfo::Iterator::operator->()
{
    return _info;
}


inline bool SerializationInfo::Iterator::operator!=(const Iterator& other) const
{
    return _info != other._info;
}


inline SerializationInfo::ConstIterator::ConstIterator()
: _info(0)
{}


inline SerializationInfo::ConstIterator::ConstIterator(const ConstIterator& other)
: _info(other._info)
{}


inline SerializationInfo::ConstIterator::ConstIterator(const SerializationInfo* info)
: _info(info)
{}


inline SerializationInfo::ConstIterator& SerializationInfo::ConstIterator::operator=(const ConstIterator& other)
{
    _info = other._info;
    return *this;
}


inline SerializationInfo::ConstIterator& SerializationInfo::ConstIterator::operator++()
{
    ++_info;
    return *this;
}


inline const SerializationInfo& SerializationInfo::ConstIterator::operator*() const
{
    return *_info;
}


inline const SerializationInfo* SerializationInfo::ConstIterator::operator->() const
{
    return _info;
}


inline bool SerializationInfo::ConstIterator::operator!=(const ConstIterator& other) const
{
    return _info != other._info;
}


inline void operator >>=(const SerializationInfo& si, bool& n)
{
    si.toValue(n);
}


inline void operator <<=(SerializationInfo& si, bool n)
{
    si.setValue(n);
    si.setTypeName("bool");
}


inline void operator >>=(const SerializationInfo& si, signed char& n)
{
    si.toValue(n);
}


inline void operator <<=(SerializationInfo& si, signed char n)
{
    si.setValue(n);
    si.setTypeName("char");
}


inline void operator >>=(const SerializationInfo& si, unsigned char& n)
{
    si.toValue(n);
}


inline void operator <<=(SerializationInfo& si, unsigned char n)
{
    si.setValue(n);
    si.setTypeName("char");
}


inline void operator >>=(const SerializationInfo& si, char& n)
{
    si.toValue(n);
}


inline void operator <<=(SerializationInfo& si, char n)
{
    si.setValue(n);
    si.setTypeName("char");
}


inline void operator >>=(const SerializationInfo& si, short& n)
{
    si.toValue(n);
}


inline void operator <<=(SerializationInfo& si, short n)
{
    si.setValue(n);
    si.setTypeName("int");
}


inline void operator >>=(const SerializationInfo& si, unsigned short& n)
{
    si.toValue(n);
}


inline void operator <<=(SerializationInfo& si, unsigned short n)
{
    si.setValue(n);
    si.setTypeName("int");
}


inline void operator >>=(const SerializationInfo& si, int& n)
{
    si.toValue(n);
}


inline void operator <<=(SerializationInfo& si, int n)
{
    si.setValue(n);
    si.setTypeName("int");
}


inline void operator >>=(const SerializationInfo& si, unsigned int& n)
{
    si.toValue(n);
}


inline void operator <<=(SerializationInfo& si, unsigned int n)
{
    si.setValue(n);
    si.setTypeName("int");
}


inline void operator >>=(const SerializationInfo& si, long& n)
{
    si.toValue(n);
}


inline void operator <<=(SerializationInfo& si, long n)
{
    si.setValue(n);
    si.setTypeName("int");
}


inline void operator >>=(const SerializationInfo& si, unsigned long& n)
{
    si.toValue(n);
}


inline void operator <<=(SerializationInfo& si, unsigned long n)
{
    si.setValue(n);
    si.setTypeName("int");
}


#ifdef HAVE_LONG_LONG

inline void operator >>=(const SerializationInfo& si, long long& n)
{
    si.toValue(n);
}


inline void operator <<=(SerializationInfo& si, long long n)
{
    si.setValue(n);
    si.setTypeName("int");
}

#endif


#ifdef HAVE_UNSIGNED_LONG_LONG

inline void operator >>=(const SerializationInfo& si, unsigned long long& n)
{
    si.toValue(n);
}


inline void operator <<=(SerializationInfo& si, unsigned long long n)
{
    si.setValue(n);
    si.setTypeName("int");
}

#endif


inline void operator >>=(const SerializationInfo& si, float& n)
{
    si.toValue<float>(n);
}


inline void operator <<=(SerializationInfo& si, float n)
{
    si.setValue(n);
    si.setTypeName("double");
}


inline void operator >>=(const SerializationInfo& si, double& n)
{
    si.toValue<double>(n);
}


inline void operator <<=(SerializationInfo& si, double n)
{
    si.setValue(n);
    si.setTypeName("double");
}


inline void operator >>=(const SerializationInfo& si, std::string& n)
{
    si.toValue<std::string>(n);
}


inline void operator <<=(SerializationInfo& si, const std::string& n)
{
    si.setValue(n);
    si.setTypeName("string");
}


inline void operator >>=(const SerializationInfo& si, cxxtools::String& n)
{
    si.toValue<cxxtools::String>(n);
}


inline void operator <<=(SerializationInfo& si, const cxxtools::String& n)
{
    si.setValue(n);
    si.setTypeName("string");
}


template <typename T, typename A>
inline void operator >>=(const SerializationInfo& si, std::vector<T, A>& vec)
{
    vec.clear();
    for(SerializationInfo::ConstIterator it = si.begin(); it != si.end(); ++it)
    {
        vec.resize( vec.size() + 1 );
        *it >>= vec.back();
    }
}


template <typename T, typename A>
inline void operator <<=(SerializationInfo& si, const std::vector<T, A>& vec)
{
    typename std::vector<T, A>::const_iterator it;

    for(it = vec.begin(); it != vec.end(); ++it)
    {
        SerializationInfo& newSi = si.addMember(std::string());
        newSi <<= *it;
    }

    si.setTypeName("array");
    si.setCategory(SerializationInfo::Array);
}


inline void operator >>=(const SerializationInfo& si, std::vector<int>& vec)
{
    vec.clear();
    for(SerializationInfo::ConstIterator it = si.begin(); it != si.end(); ++it)
    {
        vec.resize( vec.size() + 1 );
        *it >>= vec.back();
    }
}

inline void operator <<=(SerializationInfo& si, const std::vector<int>& vec)
{
    std::vector<int>::const_iterator it;

    for(it = vec.begin(); it != vec.end(); ++it)
    {
        SerializationInfo& newSi = si.addMember(std::string());
        newSi <<= *it;
    }

    si.setTypeName("array");
    si.setCategory(SerializationInfo::Array);
}


template <typename T, typename A>
inline void operator >>=(const SerializationInfo& si, std::list<T, A>& list)
{
    list.clear();
    for(SerializationInfo::ConstIterator it = si.begin(); it != si.end(); ++it)
    {
        list.resize( list.size() + 1 );
        *it >>= list.back();
    }
}


template <typename T, typename A>
inline void operator <<=(SerializationInfo& si, const std::list<T, A>& list)
{
    typename std::list<T, A>::const_iterator it;

    for(it = list.begin(); it != list.end(); ++it)
    {
        SerializationInfo& newSi = si.addMember(std::string());
        newSi <<= *it;
    }

    si.setTypeName("list");
    si.setCategory(SerializationInfo::Array);
}


template <typename T, typename A>
inline void operator >>=(const SerializationInfo& si, std::deque<T, A>& deque)
{
    deque.clear();
    for(SerializationInfo::ConstIterator it = si.begin(); it != si.end(); ++it)
    {
        deque.resize( deque.size() + 1 );
        *it >>= deque.back();
    }
}


template <typename T, typename A>
inline void operator <<=(SerializationInfo& si, const std::deque<T, A>& deque)
{
    typename std::deque<T, A>::const_iterator it;

    for(it = deque.begin(); it != deque.end(); ++it)
    {
        SerializationInfo& newSi = si.addMember(std::string());
        newSi <<= *it;
    }

    si.setTypeName("deque");
    si.setCategory(SerializationInfo::Array);
}


template <typename T, typename C, typename A>
inline void operator >>=(const SerializationInfo& si, std::set<T, C, A>& set)
{
    set.clear();
    for(SerializationInfo::ConstIterator it = si.begin(); it != si.end(); ++it)
    {
        T t;
        *it >>= t;
        set.insert(t);
    }
}


template <typename T, typename C, typename A>
inline void operator <<=(SerializationInfo& si, const std::set<T, C, A>& set)
{
    typename std::set<T, C, A>::const_iterator it;

    for(it = set.begin(); it != set.end(); ++it)
    {
        SerializationInfo& newSi = si.addMember(std::string());
        newSi <<= *it;
    }

    si.setTypeName("set");
    si.setCategory(SerializationInfo::Array);
}


template <typename T, typename C, typename A>
inline void operator >>=(const SerializationInfo& si, std::multiset<T, C, A>& multiset)
{
    multiset.clear();
    for(SerializationInfo::ConstIterator it = si.begin(); it != si.end(); ++it)
    {
        T t;
        *it >>= t;
        multiset.insert(t);
    }
}


template <typename T, typename C, typename A>
inline void operator <<=(SerializationInfo& si, const std::multiset<T, C, A>& multiset)
{
    typename std::multiset<T, C, A>::const_iterator it;

    for(it = multiset.begin(); it != multiset.end(); ++it)
    {
        SerializationInfo& newSi = si.addMember(std::string());
        newSi <<= *it;
    }

    si.setTypeName("multiset");
    si.setCategory(SerializationInfo::Array);
}


template <typename A, typename B>
inline void operator >>=(const SerializationInfo& si, std::pair<A, B>& p)
{
    si.getMember("first") >>= p.first;
    si.getMember("second") >>= p.second;
}


template <typename A, typename B>
inline void operator <<=(SerializationInfo& si, const std::pair<A, B>& p)
{
    si.addMember("first") <<= p.first;
    si.addMember("second") <<= p.second;
}


template <typename K, typename V, typename P, typename A>
inline void operator >>=(const SerializationInfo& si, std::map<K, V, P, A>& map)
{
    map.clear();
    for(SerializationInfo::ConstIterator it = si.begin(); it != si.end(); ++it)
    {
        typename std::pair<K, V> v;
        *it >>= v;
        map.insert(v);
    }
}


template <typename K, typename V, typename P, typename A>
inline void operator <<=(SerializationInfo& si, const std::map<K, V, P, A>& map)
{
    typename std::map<K, V, P, A>::const_iterator it;

    for(it = map.begin(); it != map.end(); ++it)
    {
        SerializationInfo& newSi = si.addMember(std::string());
        newSi <<= *it;
    }

    si.setTypeName("map");
    si.setCategory(SerializationInfo::Array);
}


template <typename K, typename V, typename P, typename A>
inline void operator >>=(const SerializationInfo& si, std::multimap<K, V, P, A>& multimap)
{
    multimap.clear();
    for(SerializationInfo::ConstIterator it = si.begin(); it != si.end(); ++it)
    {
        typename std::pair<K, V> v;
        *it >>= v;
        multimap.insert(v);
    }
}


template <typename T, typename C, typename P, typename A>
inline void operator <<=(SerializationInfo& si, const std::multimap<T, C, P, A>& multimap)
{
    typename std::multimap<T, C, P, A>::const_iterator it;

    for(it = multimap.begin(); it != multimap.end(); ++it)
    {
        SerializationInfo& newSi = si.addMember(std::string());
        newSi <<= *it;
    }

    si.setTypeName("multimap");
    si.setCategory(SerializationInfo::Array);
}


} // namespace cxxtools


#endif
