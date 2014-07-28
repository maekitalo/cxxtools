/*
 * Copyright (C) 2005-2008 by Dr. Marc Boris Duerner
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
#include <limits>
#include <typeinfo>
#include <cxxtools/config.h>

namespace cxxtools
{

/** @brief Represents arbitrary types during serialization.
*/
class CXXTOOLS_API SerializationInfo
{
    typedef std::vector<SerializationInfo> Nodes;

    public:
        enum Category {
            Void = 0, Value = 1, Object = 2, Array = 6
        };

        class Iterator;
        class ConstIterator;

#ifdef HAVE_LONG_LONG
        typedef long long int_type;
#else
        typedef long int_type;
#endif
#ifdef HAVE_UNSIGNED_LONG_LONG
        typedef unsigned long long unsigned_type;
#else
        typedef unsigned long unsigned_type;
#endif

    public:
        SerializationInfo();

        SerializationInfo(const SerializationInfo& si);

        ~SerializationInfo()
        { _releaseValue(); }

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

        /** @brief Serialization of flat data-types
        */
        void setValue(const String& value)       { _setString(value); }
        void setValue(const std::string& value)  { _setString8(value); }
        void setValue(const char* value)         { _setString8(value); }
        void setValue(Char value)                { _setString(String(1, value)); }
        void setValue(wchar_t value)             { _setString(String(1, value)); }
        void setValue(bool value)                { _setBool(value) ; }
        void setValue(char value)                { _setChar(value) ; }
        void setValue(unsigned char value)       { _setUInt(value) ; }
        void setValue(short value)               { _setInt(value) ; }
        void setValue(unsigned short value)      { _setUInt(value) ; }
        void setValue(int value)                 { _setInt(value) ; }
        void setValue(unsigned int value)        { _setUInt(value) ; }
        void setValue(long value)                { _setInt(value) ; }
        void setValue(unsigned long value)       { _setUInt(value) ; }
#ifdef HAVE_LONG_LONG
        void setValue(long long value)           { _setInt(value) ; }
#endif
#ifdef HAVE_UNSIGNED_LONG_LONG
        void setValue(unsigned long long value)  { _setUInt(value) ; }
#endif

        void setValue(float value)               { _setFloat(value); }
        void setValue(double value)              { _setFloat(value); }
        void setValue(long double value)         { _setFloat(value); }
        void setNull();

        /** @brief Deserialization of flat data-types
        */
        void getValue(String& value) const;
        void getValue(std::string& value) const;
        void getValue(Char& value) const               { value = _getWChar(); }
        void getValue(wchar_t& value) const            { value = _getWChar(); }
        void getValue(bool& value) const               { value = _getBool(); }
        void getValue(char& value) const               { value = _getChar(); }
        void getValue(signed char& value) const
            { value = static_cast<signed char>(_getInt("signed char", std::numeric_limits<signed char>::min(), std::numeric_limits<signed char>::max())); }
        void getValue(unsigned char& value) const
            { value = static_cast<signed char>(_getUInt("unsigned char", std::numeric_limits<unsigned char>::max())); }
        void getValue(short& value) const
            { value = static_cast<short>(_getInt("short", std::numeric_limits<short>::min(), std::numeric_limits<short>::max())); }
        void getValue(unsigned short& value) const
            { value = static_cast<unsigned short>(_getUInt("unsigned short", std::numeric_limits<unsigned short>::max())); }
        void getValue(int& value) const
            { value = static_cast<int>(_getInt("int", std::numeric_limits<int>::min(), std::numeric_limits<int>::max())); }
        void getValue(unsigned int& value) const
            { value = static_cast<unsigned int>(_getUInt("unsigned int", std::numeric_limits<unsigned int>::max())); }
        void getValue(long& value) const
            { value = static_cast<long>(_getInt("long", std::numeric_limits<long>::min(), std::numeric_limits<long>::max())); }
        void getValue(unsigned long& value) const
            { value = static_cast<unsigned long>(_getUInt("unsigned long", std::numeric_limits<unsigned long>::max())); }
#ifdef HAVE_LONG_LONG
        void getValue(long long& value) const
            { value = static_cast<long long>(_getInt("long long", std::numeric_limits<long long>::min(), std::numeric_limits<long long>::max())); }
#endif
#ifdef HAVE_UNSIGNED_LONG_LONG
        void getValue(unsigned long long& value) const
            { value = static_cast<unsigned long long>(_getUInt("unsigned long long", std::numeric_limits<unsigned long long>::max())); }
#endif
        void getValue(float& value) const
            { value = static_cast<float>(_getFloat("float", static_cast<long double>(std::numeric_limits<float>::max())*1.0000000000001)); }
        void getValue(double& value) const
            { value = static_cast<double>(_getFloat("double", static_cast<long double>(std::numeric_limits<double>::max())*1.0000000000001)); }
        void getValue(long double& value) const
            { value = static_cast<long double>(_getFloat("long double", std::numeric_limits<long double>::max())); }

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

            @throws SerializationError when member is not found.
        */
        const SerializationInfo& getMember(const std::string& name) const;

        /** @brief Deserialization of member data

            @throws std::range_error when index is too large
        */
        const SerializationInfo& getMember(unsigned idx) const;

        /** @brief Deserialization of member data.

            @return true if member is found, false otherwise.
            The passed value is not modified when the member was not found.
         */
        template <typename T>
        bool getMember(const std::string& name, T& value) const
        {
            const SerializationInfo* si = findMember(name);
            if (si == 0)
                return false;
            *si >>= value;
            return true;
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

        void swap(SerializationInfo& si);

        bool isNull() const     { return _t == t_none && _category == Void; }
        bool isString() const   { return _t == t_string; }
        bool isString8() const  { return _t == t_string8; }
        bool isChar() const     { return _t == t_char; }
        bool isBool() const     { return _t == t_bool; }
        bool isInt() const      { return _t == t_int; }
        bool isUInt() const     { return _t == t_uint; }
        bool isFloat() const    { return _t == t_float; }

        void dump(std::ostream& out, const std::string& praefix = std::string()) const;

    protected:
        void setParent(SerializationInfo& si)
        { _parent = &si; }

    private:
        SerializationInfo* _parent;
        Category _category;
        std::string _name;
        std::string _type;

        void _releaseValue();
        void _setString(const String& value);
        void _setString8(const std::string& value);
        void _setString8(const char* value);
        void _setChar(char value);
        void _setBool(bool value);
        void _setInt(int_type value);
        void _setUInt(unsigned_type value);
        void _setFloat(long double value);

        bool _getBool() const;
        wchar_t _getWChar() const;
        char _getChar() const;
        int_type _getInt(const char* type, int_type min, int_type max) const;
        unsigned_type _getUInt(const char* type, unsigned_type max) const;
        long double _getFloat(const char* type, long double max) const;

        union U
        {
            char _s[sizeof(String) >= sizeof(std::string) ? sizeof(String) : sizeof(std::string)];
            char _c;
            bool _b;
            int_type _i;
            unsigned_type _u;
            long double _f;
        } _u;

        String* _StringPtr()                    { return reinterpret_cast<String*>(_u._s); }
        String& _String()                       { return *_StringPtr(); }
        const String* _StringPtr() const        { return reinterpret_cast<const String*>(_u._s); }
        const String& _String() const           { return *_StringPtr(); }
        std::string* _String8Ptr()              { return reinterpret_cast<std::string*>(_u._s); }
        std::string& _String8()                 { return *_String8Ptr(); }
        const std::string* _String8Ptr() const  { return reinterpret_cast<const std::string*>(_u._s); }
        const std::string& _String8() const     { return *_String8Ptr(); }

        enum T
        {
          t_none,
          t_string,
          t_string8,
          t_char,
          t_bool,
          t_int,
          t_uint,
          t_float
        } _t;

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

        bool operator==(const Iterator& other) const;

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

        bool operator==(const ConstIterator& other) const;

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


inline bool SerializationInfo::Iterator::operator==(const Iterator& other) const
{
    return _info == other._info;
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


inline bool SerializationInfo::ConstIterator::operator==(const ConstIterator& other) const
{
    return _info == other._info;
}


inline void operator >>=(const SerializationInfo& si, SerializationInfo& ssi)
{
    ssi = si;
}


inline void operator <<=(SerializationInfo& si, const SerializationInfo& ssi)
{
    si = ssi;
}


inline void operator >>=(const SerializationInfo& si, bool& n)
{
    si.getValue(n);
}


inline void operator <<=(SerializationInfo& si, bool n)
{
    si.setValue(n);
    si.setTypeName("bool");
}


inline void operator >>=(const SerializationInfo& si, signed char& n)
{
    si.getValue(n);
}


inline void operator <<=(SerializationInfo& si, signed char n)
{
    si.setValue(n);
    si.setTypeName("char");
}


inline void operator >>=(const SerializationInfo& si, unsigned char& n)
{
    si.getValue(n);
}


inline void operator <<=(SerializationInfo& si, unsigned char n)
{
    si.setValue(n);
    si.setTypeName("char");
}


inline void operator >>=(const SerializationInfo& si, char& n)
{
    si.getValue(n);
}


inline void operator <<=(SerializationInfo& si, char n)
{
    si.setValue(n);
    si.setTypeName("char");
}


inline void operator >>=(const SerializationInfo& si, short& n)
{
    si.getValue(n);
}


inline void operator <<=(SerializationInfo& si, short n)
{
    si.setValue(n);
    si.setTypeName("int");
}


inline void operator >>=(const SerializationInfo& si, unsigned short& n)
{
    si.getValue(n);
}


inline void operator <<=(SerializationInfo& si, unsigned short n)
{
    si.setValue(n);
    si.setTypeName("int");
}


inline void operator >>=(const SerializationInfo& si, int& n)
{
    si.getValue(n);
}


inline void operator <<=(SerializationInfo& si, int n)
{
    si.setValue(n);
    si.setTypeName("int");
}


inline void operator >>=(const SerializationInfo& si, unsigned int& n)
{
    si.getValue(n);
}


inline void operator <<=(SerializationInfo& si, unsigned int n)
{
    si.setValue(n);
    si.setTypeName("int");
}


inline void operator >>=(const SerializationInfo& si, long& n)
{
    si.getValue(n);
}


inline void operator <<=(SerializationInfo& si, long n)
{
    si.setValue(n);
    si.setTypeName("int");
}


inline void operator >>=(const SerializationInfo& si, unsigned long& n)
{
    si.getValue(n);
}


inline void operator <<=(SerializationInfo& si, unsigned long n)
{
    si.setValue(n);
    si.setTypeName("int");
}


#ifdef HAVE_LONG_LONG

inline void operator >>=(const SerializationInfo& si, long long& n)
{
    si.getValue(n);
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
    si.getValue(n);
}


inline void operator <<=(SerializationInfo& si, unsigned long long n)
{
    si.setValue(n);
    si.setTypeName("int");
}

#endif


inline void operator >>=(const SerializationInfo& si, float& n)
{
    si.getValue(n);
}


inline void operator <<=(SerializationInfo& si, float n)
{
    si.setValue(n);
    si.setTypeName("double");
}


inline void operator >>=(const SerializationInfo& si, double& n)
{
    si.getValue(n);
}


inline void operator <<=(SerializationInfo& si, double n)
{
    si.setValue(n);
    si.setTypeName("double");
}


inline void operator >>=(const SerializationInfo& si, std::string& n)
{
    si.getValue(n);
}


inline void operator <<=(SerializationInfo& si, const std::string& n)
{
    si.setValue(n);
    si.setTypeName("string");
}


inline void operator <<=(SerializationInfo& si, const char* n)
{
    si.setValue(n);
    si.setTypeName("string");
}


inline void operator >>=(const SerializationInfo& si, cxxtools::String& n)
{
    si.getValue(n);
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
    vec.reserve(si.memberCount());
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

    si.reserve(vec.size());
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
    si.setTypeName("pair");
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


inline std::ostream& operator<< (std::ostream& out, const SerializationInfo& si)
{
    si.dump(out);
    return out;
}

} // namespace cxxtools


#endif
