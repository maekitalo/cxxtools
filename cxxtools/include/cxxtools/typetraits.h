/***************************************************************************
 *   Copyright (C) 2008 Marc Boris Duerner                                 *
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
#ifndef CXXTOOLS_TypeTraits_h
#define CXXTOOLS_TypeTraits_h

#include <cstddef>

namespace cxxtools {

    template <typename T>
    struct TypeTraitsBase {
        typedef T Value;
        typedef const T ConstValue;
        typedef T& Reference;
        typedef const T& ConstReference;
        typedef T* Pointer;
        typedef const T* ConstPointer;
    };

    /** @brief Type-traits for for non-const value types

        Compile time type information (CTTI) is implemented in Pt by the
        means of TypeTraits. A number of specialisations allows
        compile type branching in gerneric code depending on the type.
    */
    template <typename T>
    struct TypeTraits : public TypeTraitsBase<T>
    {
        static const unsigned int isConst = 0;
        static const unsigned int isPointer = 0;
        static const unsigned int isReference = 0;
    };

    /** @brief Type-traits for for const value types
    */
    template <typename T>
    struct TypeTraits<const T> : public TypeTraitsBase<T>
    {
        static const unsigned int isConst = 1;
        static const unsigned int isPointer = 0;
        static const unsigned int isReference = 0;
    };

    /** @brief Type-traits for for non-const reference types
    */
    template <typename T>
    struct TypeTraits<T&> : public TypeTraitsBase<T>
    {
        static const unsigned int isConst = 0;
        static const unsigned int isPointer = 0;
        static const unsigned int isReference = 1;
    };

    /** @brief Type-traits for for const reference types
    */
    template <typename T>
    struct TypeTraits<const T&> : public TypeTraitsBase<T>
    {
        static const unsigned int isConst = 1;
        static const unsigned int isPointer = 0;
        static const unsigned int isReference = 1;
    };

    /** @brief Type-traits for for non-const pointer types
    */
    template <typename T>
    struct TypeTraits<T*> : public TypeTraitsBase<T>
    {
        static const unsigned int isConst = 0;
        static const unsigned int isPointer = 1;
        static const unsigned int isReference = 0;
    };

    /** @brief Type-traits for for const pointer types
    */
    template <typename T>
    struct TypeTraits<const T*> : public TypeTraitsBase<T>
    {
        static const unsigned int isConst = 1;
        static const unsigned int isPointer = 1;
        static const unsigned int isReference = 0;
    };

    /** @brief Type-traits for for fixed-size array types
    */
    template <typename T, std::size_t N>
    struct TypeTraits<T[N]> : public TypeTraitsBase<T>
    {
        static const unsigned int isConst = 0;
        static const unsigned int isPointer = 1;
        static const unsigned int isReference = 0;
    };

    /** @brief Type-traits for for void
    */
    template <>
    struct TypeTraits<void>
    {
        typedef void Value;
        typedef void ConstType;
        typedef void Reference;
        typedef void ConstReference;
        typedef void* Pointer;
        typedef void* ConstPointer;

        static const unsigned int isConst = 0;
        static const unsigned int isPointer = 0;
        static const unsigned int isReference = 0;
    };

} // namespace cxxtools

#endif
