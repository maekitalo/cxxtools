/***************************************************************************
 *   Copyright (C) 2004-2007 by Marc Boris Duerner                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef cxxtools_Any_h
#define cxxtools_Any_h

#include <cxxtools/typeinfo.h>
#include <typeinfo>
#include <cstring>

namespace cxxtools {

    /** @brief Contains an arbitrary type
        @ingroup Reflection

        Any can contain any other type that is default- and copy constructible
        and less-than and equality comparable. When a value is assigned to an
        Any a copy is made, just like when a type is inserted in a standard
        C++ container. The contained type can be accessed via Pt::any_cast<>.
         It is only possible to get the contained value if the type matches

        @code
            Any a = 5;
            int i = any_cast<int>( a );    // i is 5 now
            float f = any_cast<float>( a ) // throws std::bad_cast
        @endcode

        Anys can be compared by the contained types and values. Two Anys are
        considered equal when the contained values are equal and of the same
        type. A special case is less-than comparison, when the contained
        types are different. std::type_info::before will be used to decide
        which Any is less.

        @code
            Any a = 6;
            Any b = 6;
            Any c = '6';
            Any d = 1;

            // true, same type, same value
            a == b;

            // false, different types
            b == c;

            // true, same type and less
            d \< a;

            // implementation dependent
            d \< c;
        @endcode
    */
    class Any
    {
        template <typename T>
        friend T any_cast(const Any&);

        public:
            /** @internal
            */
            class Value
            {
                public:
                    virtual ~Value() {}
                    virtual Value* clone() const = 0;
                    virtual const char* typeName() const = 0;
                    virtual const std::type_info& type() const = 0;
                    virtual bool equal(const Value& value) const = 0;
                    virtual bool lt(const Value& value) const = 0;
            };

            /** @internal
            */
            template <typename T>
            class BasicValue : public Value
            {
                public:
                    BasicValue(const T& value = T())
                    : _value(value)
                    { }

                    const T& value() const
                    { return _value;}

                    T& value()
                    { return _value;}

                    virtual const char* typeName() const
                    { return typeid(T).name(); }

                    virtual const std::type_info& type() const
                    { return typeid(T); }

                    virtual Any::Value* clone() const
                    { return new BasicValue(_value); }

                    virtual bool equal(const Value& value) const
                    {
                        try {
                            const BasicValue<T>& bv = dynamic_cast< const BasicValue<T>& >(value);
                            return (bv._value == this->_value);
                        }
                        catch(...) {}

                        return false;
                    }

                    virtual bool lt(const Value& value) const
                    {
                        try {
                            const BasicValue<T>& bv = dynamic_cast< const BasicValue<T>& >(value);
                            return (bv._value < this->_value);
                        }
                        catch(...) {}

                        bool x = !( typeid(T).before( value.type() ) );
                        return !x;
                    }

                private:
                    T _value;
            };

        public:
            /** @brief Construct with value

                Constructs the Any from an value of arbitrary type. The type
                to be assigned must be copy-constructible. Memory is allocated
                to store the value. If an exception is thrown during
                construction, the Any will be empty and the exception is
                porpagated.

                @param type Value to assign
            */
            template <typename T>
            Any(const T& type)
            : _value(0)
            { (*this) = type; }

            /** @brief Default constructor

                Constructs an empty any. No memory needs to be allocated for
                empty Anys.
            */
            Any();

            /** @brief Assigns an abstract value
            */
            Any& assign(Value* value);

            /** @brief Copy constructor

                Constructs the Any by copying the value of the other Any. It
                is legal to assign an empty Any. If an exception is thrown
                during construction, the Any will be empty and the exception
                is porpagated.

                @param val Any to assign
            */
            Any(const Any& val);

            /** @brief Destructor

                Deallocates the memory needed to hold the value. This will
                also destruct the contained type.
            */
            ~Any();

            /** @brief Clear content

                Removes the stored type resulting in a destructor call
                for the stored type. All memory required to hold the value
                is deallocated.
            */
            void clear();

            /** @brief Check if empty

                Returns true if no value has been assigned, false otherwise.

                @return True if empty
            */
            inline bool empty() const
            { return !_value; }

            /** @brief Swap values

                The member function swaps the assigned values between *this and right.
                No exceptions are thrown, and no memory needs to be allocated.

                @param other Other any to swap value
                @return self reference
            */
            Any& swap(Any& other);

            /** @brief Check typename of assigned type

                Returns the typename of the currently assigned type. If the
                Any is empty "void" is returned.

                @return Typename
            */
            const char* typeName() const
            { return _value ? _value->typeName() : "void"; }

            /** @brief Returns type info of assigned type

                Returns the std::type_info of the currently assigned type. If the
                Any is empty the type_info of void is returned.

                @return Type info
            */
            const std::type_info& type() const
            { return _value ? _value->type() : typeid(void); }

            /** @brief Assign value

                Assigns a value of an arbitrary type. The type to be assigned
                must be copy-constructible. Memory is allocated to store the value.
                If an exception is thrown during construction, the Any will remain
                unaltered and the exception is porpagated.

                @param rhs Value to assign
            */
            template <typename T>
            Any& operator=(const T& rhs)
            {
                Any::Value* tmp = new BasicValue<T>(rhs);
                delete _value;
                _value = tmp;
                return *this;
            }

            /** @brief Assign value of other Any

                Assignes the value of another Any by copying the value of the
                other Any. It is legal to assign an empty Any. If an exception
                is thrown during assignment, the Any will remain unchanged and
                the exception is porpagated.

                @param rhs Any to assign
            */
            Any& operator=(const Any& rhs);

            /** @brief Check if equal

                Returns true if the contained type and the passed type are
                equal and have equal values.

                @return True if equal
            */
            template <typename T>
            bool operator==(const T& value) const
            {
                if(_value == 0)
                    return false;

                return _value->equal( BasicValue<T>(value) );
            }

            /** @brief Check if equal

                Returns true if the contained types are equal and have
                equal values.

                @return True if equal
            */
            bool operator==(const Any& a) const;

            /** @brief Check if inequal

                Returns true if the contained types have different values
                or if the conatained types are different.

                @return True if different
            */
            bool operator!=(const Any& a) const;

            /** @brief Check if less

                Returns true if the value of the contained type is less than
                the contained value of the other Any. If the contained types
                are different type_info::before decides which Any is less.

                @return True if less
            */
            bool operator<(const Any& a) const;

            const Any::Value* value() const
            { return _value; }

            Any::Value* value()
            { return _value; }

        private:
            /** @internal */
            Value* _value;
    };


    /** @brief Get contained value

        This function is used to get the contained value from an Any. It is
        not possible to get a float out of an Any if the contained value is
        an int, but the typeid's must match. It is, however, possible to
        get a const reference ton the contained type.

        @param any Any to read to
        @return contained value
        @throw std::bad_cast on type mismatch
    */
    template <typename T>
    inline T any_cast(const Any& any)
    {
        // NOTE:
        // This implementation is a workaround:
        // - the first if(...) may not work properly on Linux when loading libs,
        //   so there is also a comparison of string names (second if(...))
        // - but: the name() method necessary for string comparison does not
        //   exist on WinCE, so the second if(...) is not compiled for WinCE

        typedef typename TypeInfo<T>::Value ValueT;

        if( any.type() == typeid(ValueT) )
        {
            const Any::BasicValue<ValueT>* value;
            value = static_cast< const Any::BasicValue<ValueT>* >(any._value);
            return value->value();
        }

#ifndef _WIN32_WCE
        else if( 0 == strcmp( any.type().name(), typeid(ValueT).name() ) )
        {
            const Any::BasicValue<ValueT>* value;
            value = static_cast< const Any::BasicValue<ValueT>* >(any._value);
            return value->value();
        }
#endif

        throw std::bad_cast();
    }

}

#endif
