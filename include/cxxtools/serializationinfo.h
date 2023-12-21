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

#include <cxxtools/string.h>
#include <vector>
#include <set>
#include <map>
#include <list>
#include <deque>
#include <limits>
#include <forward_list>
#include <unordered_set>
#include <unordered_map>
#include <tuple>
#include <array>
#include <type_traits>
#include <iosfwd>

#include <cxxtools/config.h>

#if __cplusplus >= 201703L
#include <optional>
#endif

namespace cxxtools
{

/** @brief Represents arbitrary types during serialization.

    A SerializationInfo represent arbitrary types during serialization. It is
    a flexible structure, which can hold all informations to reconstruct a
    object.

    The types of informations are:

     - a category (Void, Value, Object or Array)
     - a scalar value
     - a name of the type
     - a ordered list of SerializationInfo objects

    Those informations are filled when objects are serialized using a
    serialization operator and a serializer can use those to create a
    serialized representation of a object.

    A deserializer is used to convert a serialized representation into a
    SerializationInfo and a deserialization operator is used to convert the
    SerializationInfo back to the desired object.

    To serialize and deserialize a object, those operators are needed.
    For all standard types there are predefined operators. For user types
    like own classes a operator can easily be written. The signature is
    always the same.

    Serialization operator is defined as:

        void operator <<= (cxxtools::SerializationInfo& si, const YourType& object);

    It must collect all informations from `YourType` and put them to the
    cxxtools::SerializationInfo.

    The deserialization operator is defined as:

        void operator >>= (const cxxtools::SerializationInfo& si, YourType& object);

    It must fetch the informations from SerializationInfo and fill the passed object
    to reconstruct it.

    With those 2 operators a serializer is able to to convert the object to
    some format and the deserializers to convert it back to the object.

    Since for standard container types operators are defined using templates,
    The operators above are also able to serializer and deserialize e.g. a
    `std::vector<YourType>`.

    The scalar value may have several types. When accessing the value it is
    typically converted implicitly when possible. So when e.g. the
    SerializationInfo is set to a numeric value and a string value is read, you
    get a string representation of the numeric value.

 */

class SerializationInfo
{
        typedef std::deque<SerializationInfo> Nodes;
        friend void operator <<=(SerializationInfo& si, const SerializationInfo& ssi);

    public:
        enum Category {
            Void = 0, Value = 1, Object = 2, Array = 6
        };

        /// iterator over subnodes.
        typedef Nodes::iterator Iterator;
        /// const iterator over subnodes.
        typedef Nodes::const_iterator ConstIterator;

        /// iterator over subnodes, std style name.
        typedef Nodes::iterator iterator;
        /// const iterator over subnodes, std style name.
        typedef Nodes::const_iterator const_iterator;

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

        ~SerializationInfo();

        SerializationInfo& operator =(const SerializationInfo& si);

        SerializationInfo(SerializationInfo&& si) noexcept;

        SerializationInfo& operator=(SerializationInfo&& si);

        Category category() const
        {
            return _category;
        }

        void setCategory(Category cat)
        {
            _category = cat;
        }

        const std::string& typeName() const
        {
            return _type;
        }

        void setTypeName(const std::string& type)
        {
            _type = type;
            if (_category == Void)
                _category = Object;
        }

        void setTypeName(std::string&& type)
        {
            _type = std::move(type);
            if (_category == Void)
                _category = Object;
        }

        const std::string& name() const
        {
            return _name;
        }

        void setName(const std::string& name)
        {
            _name = name;
        }

        void setName(std::string&& name)
        {
            _name = std::move(name);
        }

        /** @brief Serialization of flat data-types
        */
        void setValue(String&& value)            { _setString(std::move(value)); }
        void setValue(std::string&& value)       { _setString8(std::move(value)); }
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
        void setValue(double value)              { _setDouble(value); }
        void setValue(long double value)         { _setLongDouble(value); }
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
            { value = _getFloat(); }
        void getValue(double& value) const
            { value = _getDouble(); }
        void getValue(long double& value) const
            { value = _getLongDouble(); }

        /** @brief Serialization of flat member data-types
        */
        template <typename T>
        SerializationInfo& addValue(const std::string& name, const T& value)
        {
            SerializationInfo& info = this->addMember(name);
            info.setValue(T(value));
            return info;
        }

        /** @brief Serialization of member data
        */
        SerializationInfo& addMember(const std::string& name = std::string());

        /** @brief Returns a member; adds when not yet found
         */
        SerializationInfo& getAddMember(const std::string& name)
        {
            SerializationInfo* si = findMember(name);
            if (si == 0)
                return addMember(name);
            return *si;
        }

        /** @brief Deserialization of member data

            @throws SerializationError when member is not found.
        */
        const SerializationInfo& getMember(const std::string& name) const;
        SerializationInfo& getMember(const std::string& name);

        /** @brief Deserialization of member data

            @throws std::range_error when index is too large
        */
        const SerializationInfo& getMember(unsigned idx) const;
        SerializationInfo& getMember(unsigned idx);

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
            return nodes().size();
        }

        Iterator begin()
        {
            return nodes().begin();
        }

        Iterator end()
        {
            return nodes().end();
        }

        ConstIterator begin() const
        {
            return nodes().begin();
        }

        ConstIterator end() const
        {
            return nodes().end();
        }

        void clear();

        void swap(SerializationInfo& si);

        bool isNull() const       { return _t == t_none && (_category == Void || _category == Value); }
        bool isString() const     { return _t == t_string; }
        bool isString8() const    { return _t == t_string8; }
        bool isChar() const       { return _t == t_char; }
        bool isBool() const       { return _t == t_bool; }
        bool isInt() const        { return _t == t_int; }
        bool isUInt() const       { return _t == t_uint; }
        bool isFloat() const      { return _t == t_float; }
        bool isDouble() const     { return _t == t_double; }
        bool isLongDouble() const { return _t == t_ldouble; }

        /** fetch part of SerializationInfo using a path notation

                $           root element
                .member     access member
                [n]         array index
                ::size      size
                ::type      typename
                ::isnull    true if null, false otherwise

                Root element can be omitted. Also the starting '.'

                `$.store.book` can be written as `store.book`
                `$[2]` can be written as `[2]`

            examples

                $.store.book::size
                $.store.book[2].title
                $.store.book[2].price
                $::size
         */
        SerializationInfo path(const std::string& path) const;

        void dump(std::ostream& out, const std::string& prefix = std::string()) const;

    private:
        Category _category;
        std::string _name;
        std::string _type;

        void _releaseValue();
        void _setString(String&& value);
        void _setString8(std::string&& value);
        void _setString8(const char* value);
        void _setChar(char value);
        void _setBool(bool value);
        void _setInt(int_type value);
        void _setUInt(unsigned_type value);
        void _setFloat(float value);
        void _setDouble(double value);
        void _setLongDouble(long double value);

        bool _getBool() const;
        wchar_t _getWChar() const;
        char _getChar() const;
        int_type _getInt(const char* type, int_type min, int_type max) const;
        unsigned_type _getUInt(const char* type, unsigned_type max) const;
        float _getFloat() const;
        double _getDouble() const;
        long double _getLongDouble() const;
        Nodes& nodes();
        const Nodes& nodes() const;
        // assignment without name
        void assignData(const SerializationInfo& si);

        union U
        {
            char _s[sizeof(String) >= sizeof(std::string) ? sizeof(String) : sizeof(std::string)];
            char _c;
            bool _b;
            int_type _i;
            unsigned_type _u;
            float _f;
            double _d;
            long double _ld;
        } _u;

        String* _StringPtr()                    { return reinterpret_cast<String*>(&_u); }
        String& _StringRef()                    { return *_StringPtr(); }
        String&& _String()                      { return std::move(*_StringPtr()); }
        const String* _StringPtr() const        { return reinterpret_cast<const String*>(&_u); }
        const String& _String() const           { return *_StringPtr(); }
        std::string* _String8Ptr()              { return reinterpret_cast<std::string*>(&_u); }
        std::string& _String8Ref()              { return *_String8Ptr(); }
        std::string&& _String8()                { return std::move(*_String8Ptr()); }
        const std::string* _String8Ptr() const  { return reinterpret_cast<const std::string*>(&_u); }
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
          t_float,
          t_double,
          t_ldouble
        } _t;

        Nodes* _nodes;             // objects/arrays
};


inline SerializationInfo::SerializationInfo()
: _category(Void),
  _t(t_none),
  _nodes(0)
{ }


inline void operator >>=(const SerializationInfo& si, SerializationInfo& ssi)
{
    ssi = si;
}


void operator <<=(SerializationInfo& si, const SerializationInfo& ssi);


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
    si.setTypeName("float");
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


inline void operator >>=(const SerializationInfo& si, long double& n)
{
    si.getValue(n);
}


inline void operator <<=(SerializationInfo& si, long double n)
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
    si.setValue(std::string(n));
    si.setTypeName("string");
}


inline void operator <<=(SerializationInfo& si, const char* n)
{
    si.setValue(n);
    si.setTypeName("string");
}


inline void operator >>=(const SerializationInfo& si, String& n)
{
    si.getValue(n);
}


inline void operator <<=(SerializationInfo& si, const String& n)
{
    si.setValue(String(n));
    si.setTypeName("string");
}


inline void operator >>=(const SerializationInfo& si, Char& n)
{
    si.getValue(n);
}


inline void operator <<=(SerializationInfo& si, const Char& n)
{
    si.setValue(n);
    si.setTypeName("char");
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

    for(it = vec.begin(); it != vec.end(); ++it)
    {
        SerializationInfo& newSi = si.addMember();
        newSi <<= *it;
    }

    si.setTypeName("array");
    si.setCategory(SerializationInfo::Array);
}


inline void operator>>=(const SerializationInfo& si, std::vector<bool>::reference bit)
{
    bool v;
    si >>= v;
    bit = v;
}


inline void operator<<=(SerializationInfo& si, std::vector<bool>::reference bit)
{
    si <<= static_cast<bool>(bit);
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
        SerializationInfo& newSi = si.addMember();
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
        SerializationInfo& newSi = si.addMember();
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
        SerializationInfo& newSi = si.addMember();
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
        SerializationInfo& newSi = si.addMember();
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
        typename std::map<K, V, P, A>::value_type vv(v.first, v.second);
        map.insert(vv);
    }
}


template <typename K, typename V, typename P, typename A>
inline void operator <<=(SerializationInfo& si, const std::map<K, V, P, A>& map)
{
    typename std::map<K, V, P, A>::const_iterator it;

    for(it = map.begin(); it != map.end(); ++it)
    {
        SerializationInfo& newSi = si.addMember();
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
        typename std::multimap<K, V, P, A>::value_type vv(v.first, v.second);
        multimap.insert(vv);
    }
}


template <typename T, typename C, typename P, typename A>
inline void operator <<=(SerializationInfo& si, const std::multimap<T, C, P, A>& multimap)
{
    typename std::multimap<T, C, P, A>::const_iterator it;

    for(it = multimap.begin(); it != multimap.end(); ++it)
    {
        SerializationInfo& newSi = si.addMember();
        newSi <<= *it;
    }

    si.setTypeName("multimap");
    si.setCategory(SerializationInfo::Array);
}

template <typename T, size_t N>
void operator >>=(const SerializationInfo& si, T (&v)[N])
{
    for (size_t n = 0; n < N; ++n)
        si.getMember(n) >>= v[n];
}

template <typename T, size_t N>
void operator <<=(SerializationInfo& si, const T (&v)[N])
{
    for (size_t n = 0; n < N; ++n)
        si.addMember() <<= v[n];

    si.setTypeName("array");
    si.setCategory(SerializationInfo::Array);
}

template <typename T, typename A>
inline void operator >>=(const SerializationInfo& si, std::forward_list<T, A>& list)
{
    list.clear();
    for (size_t n = si.memberCount(); n > 0; --n)
    {
        list.push_front(T());
        si.getMember(n-1) >>= list.front();
    }
}


template <typename T, typename A>
inline void operator <<=(SerializationInfo& si, const std::forward_list<T, A>& list)
{
    typename std::forward_list<T, A>::const_iterator it;

    for(it = list.begin(); it != list.end(); ++it)
    {
        SerializationInfo& newSi = si.addMember();
        newSi <<= *it;
    }

    si.setTypeName("list");
    si.setCategory(SerializationInfo::Array);
}


template <typename T, typename H, typename P, typename A>
inline void operator >>=(const SerializationInfo& si, std::unordered_set<T, H, P, A>& unordered_set)
{
    unordered_set.clear();
    for(SerializationInfo::ConstIterator it = si.begin(); it != si.end(); ++it)
    {
        T t;
        *it >>= t;
        unordered_set.insert(t);
    }
}


template <typename T, typename H, typename P, typename A>
inline void operator <<=(SerializationInfo& si, const std::unordered_set<T, H, P, A>& unordered_set)
{
    typename std::unordered_set<T, H, P, A>::const_iterator it;

    for(it = unordered_set.begin(); it != unordered_set.end(); ++it)
    {
        SerializationInfo& newSi = si.addMember();
        newSi <<= *it;
    }

    si.setTypeName("set");
    si.setCategory(SerializationInfo::Array);
}


template <typename T, typename H, typename P, typename A>
inline void operator >>=(const SerializationInfo& si, std::unordered_multiset<T, H, P, A>& unordered_multiset)
{
    unordered_multiset.clear();
    for(SerializationInfo::ConstIterator it = si.begin(); it != si.end(); ++it)
    {
        T t;
        *it >>= t;
        unordered_multiset.insert(t);
    }
}


template <typename T, typename H, typename P, typename A>
inline void operator <<=(SerializationInfo& si, const std::unordered_multiset<T, H, P, A>& unordered_multiset)
{
    typename std::unordered_multiset<T, H, P, A>::const_iterator it;

    for(it = unordered_multiset.begin(); it != unordered_multiset.end(); ++it)
    {
        SerializationInfo& newSi = si.addMember();
        newSi <<= *it;
    }

    si.setTypeName("set");
    si.setCategory(SerializationInfo::Array);
}


template <typename K, typename V, typename H, typename P, typename A>
inline void operator >>=(const SerializationInfo& si, std::unordered_map<K, V, H, P, A>& unordered_map)
{
    unordered_map.clear();
    for(SerializationInfo::ConstIterator it = si.begin(); it != si.end(); ++it)
    {
        typename std::pair<K, V> v;
        *it >>= v;
        typename std::unordered_map<K, V, H, P, A>::value_type vv(v.first, v.second);
        unordered_map.insert(vv);
    }
}


template <typename K, typename V, typename H, typename P, typename A>
inline void operator <<=(SerializationInfo& si, const std::unordered_map<K, V, H, P, A>& unordered_map)
{
    typename std::unordered_map<K, V, H, P, A>::const_iterator it;

    for(it = unordered_map.begin(); it != unordered_map.end(); ++it)
    {
        SerializationInfo& newSi = si.addMember();
        newSi <<= *it;
    }

    si.setTypeName("map");
    si.setCategory(SerializationInfo::Array);
}


template <typename K, typename V, typename H, typename P, typename A>
inline void operator >>=(const SerializationInfo& si, std::unordered_multimap<K, V, H, P, A>& unordered_multimap)
{
    unordered_multimap.clear();
    for(SerializationInfo::ConstIterator it = si.begin(); it != si.end(); ++it)
    {
        typename std::pair<K, V> v;
        *it >>= v;
        typename std::unordered_multimap<K, V, H, P, A>::value_type vv(v.first, v.second);
        unordered_multimap.insert(vv);
    }
}


template <typename K, typename V, typename H, typename P, typename A>
inline void operator <<=(SerializationInfo& si, const std::unordered_multimap<K, V, H, P, A>& unordered_multimap)
{
    typename std::unordered_multimap<K, V, H, P, A>::const_iterator it;

    for(it = unordered_multimap.begin(); it != unordered_multimap.end(); ++it)
    {
        SerializationInfo& newSi = si.addMember();
        newSi <<= *it;
    }

    si.setTypeName("map");
    si.setCategory(SerializationInfo::Array);
}

namespace helper
{
    template <int index, typename ... Types>
    struct TupleSerializer
    {
        void serialize(SerializationInfo& si, const std::tuple<Types...>& tuple) const
        {
            TupleSerializer<index - 1, Types...>{}.serialize(si, tuple);
            si.addMember() <<= std::get<index>(tuple);
        }

        void deserialize(const SerializationInfo& si, std::tuple<Types...>& tuple) const
        {
            TupleSerializer<index - 1, Types...>{}.deserialize(si, tuple);
            si.getMember(index) >>= std::get<index>(tuple);
        }
    };

    template <typename ... Types>
    struct TupleSerializer<0, Types ...>
    {
        void serialize(SerializationInfo& si, const std::tuple<Types...>& tuple) const
        {
            si.addMember() <<= std::get<0>(tuple);
        }

        void deserialize(const SerializationInfo& si, std::tuple<Types...>& tuple) const
        {
            si.getMember(0) >>= std::get<0>(tuple);
        }
    };
}

template <class... Types>
void operator >>=(const SerializationInfo& si, std::tuple<Types...>& tuple)
{
    const auto size = std::tuple_size<std::tuple<Types...>>::value;
    helper::TupleSerializer<size - 1, Types...>{}.deserialize(si, tuple);
}

template <typename ... Types>
void operator <<=(SerializationInfo& si, const std::tuple<Types...>& tuple)
{
    const auto size = std::tuple_size<std::tuple<Types...>>::value;
    helper::TupleSerializer<size - 1, Types...>{}.serialize(si, tuple);

    si.setTypeName("tuple");
    si.setCategory(SerializationInfo::Array);
}

template <typename T, size_t N>
void operator >>=(const SerializationInfo& si, std::array<T, N>& array)
{
    for (size_t n = 0; n < N; ++n)
        si.getMember(n) >>= array[n];
}

template <typename T, size_t N>
void operator <<=(SerializationInfo& si, const std::array<T, N>& array)
{
    for (size_t n = 0; n < N; ++n)
        si.addMember() <<= array[n];

    si.setTypeName("array");
    si.setCategory(SerializationInfo::Array);
}

#if __cplusplus >= 201703L

template <typename T>
inline void operator >>=(const SerializationInfo& si, std::optional<T>& value)
{
    if (si.isNull())
    {
        value = std::nullopt;
    }
    else
    {
        value = T();
        si >>= *value;
    }
}

template <typename T>
inline void operator <<=(SerializationInfo& si, const std::optional<T>& value)
{
    if (value)
        si <<= *value;
    else
        si.setNull();
}

#endif

/// @internal
template <typename ENUM>
class EnumClassSerializer
{
    const ENUM& _value;

public:
    explicit EnumClassSerializer(const ENUM& e)
        : _value(e)
        { }

    const ENUM& value() const   { return _value; }
};

/// @internal
template <typename ENUM>
void operator <<=(SerializationInfo& si, EnumClassSerializer<ENUM> e)
{
    si <<= static_cast<typename std::underlying_type<ENUM>::type>(e.value());
}

/// @internal
template <typename ENUM>
class EnumClassDeserializer
{
    ENUM& _value;

public:
    explicit EnumClassDeserializer(ENUM& e)
        : _value(e)
        { }

    ENUM& value() const   { return _value; }
};

/// @internal
template <typename ENUM>
void operator <<=(SerializationInfo& si, EnumClassDeserializer<ENUM> e)
{
    si <<= static_cast<typename std::underlying_type<ENUM>::type>(e.value());
}

/// @internal
template <typename ENUM>
void operator >>=(const SerializationInfo& si, EnumClassDeserializer<ENUM> e)
{
    si >>= reinterpret_cast<typename std::underlying_type<ENUM>::type&>(e.value());
}

/** Helper for serializing strongly typed enums.
 
    To serialize a enum this function returns a helper class, which can
    serialize the underlying type using the serialization operator.

    @code
        enum class Foo : char
        {
            FOO, BAR, BAZ
        };

        Foo foo(Foo::FOO);

        cxxtools::SerializationInfo si;
        si <<= cxxtools::EnumClass(foo);
    @endcode

 */
template <typename ENUM>
EnumClassSerializer<ENUM> EnumClass(const ENUM& e)
{ return EnumClassSerializer<ENUM>(e); }

/** Helper for deserializing strongly typed enums.
 
    To deserialize a enum this function returns a helper class, which can
    serialize and deserialize the underlying type using the proper operators.

    @code
        enum class Foo : char
        {
            FOO, BAR, BAZ
        };

        Foo foo(Foo::FOO);

        cxxtools::SerializationInfo si;
        si >>= cxxtools::EnumClass(foo);
    @endcode

 */
template <typename ENUM>
EnumClassDeserializer<ENUM> EnumClass(ENUM& e)
{ return EnumClassDeserializer<ENUM>(e); }

inline std::ostream& operator<< (std::ostream& out, const SerializationInfo& si)
{
    si.dump(out);
    return out;
}

/** Converts one object to another using serialization operators.

    If a object is serializable (i.e. it has a operator<<=(SerializationInfo&, T)),
    and another is deserializable (i.e. it has a operator>>=(const SerializationInfo&, ...))
    this function can be used to convert one to the other.

    @code
        std::set<int> foo = getSetFromSomewhere();
        std::vector<long> bar = cxxtools::serialization_cast<std::vector<long> >(foo);
        // or with C++11:
        auto bar = cxxtools::serialization_cast<std::vector<long> >(foo);
    @endcode
 */
template <typename R, typename T>
R serialization_cast(const T& t)
{
    SerializationInfo si;
    si <<= t;
    R r;
    si >>= r;
    return r;
}

} // namespace cxxtools


#endif
