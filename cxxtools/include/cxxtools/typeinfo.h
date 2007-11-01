/***************************************************************************
 *   Copyright (C) 2005 Marc Boris Duerner                                 *
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
#ifndef cxxtools_TypeInfo_h
#define cxxtools_TypeInfo_h

#include <cstddef>

namespace cxxtools {

    template <typename T>
    struct TypeInfoBase
    {
        typedef T Value;
        typedef const T ConstType;
        typedef T& Reference;
        typedef const T& ConstReference;
        typedef T* Pointer;
        typedef const T* ConstPointer;
    };


    /** @brief CTTI for for non-const value types
        @ingroup CTTI
    */
    template <typename T>
    struct TypeInfo : public TypeInfoBase<T>
    {
    };


    /** @brief CTTI for for const value types
        @ingroup CTTI
    */
    template <typename T>
    struct TypeInfo<const T> : public TypeInfoBase<T>
    {
    };


    /** @brief CTTI for for non-const reference types
        @ingroup CTTI
    */
    template <typename T>
    struct TypeInfo<T&> : public TypeInfoBase<T>
    {
    };


    /** @brief CTTI for for const reference types
        @ingroup CTTI
    */
    template <typename T>
    struct TypeInfo<const T&> : public TypeInfoBase<T>
    {
    };


    /** @brief CTTI for for non-const pointer types
        @ingroup CTTI
    */
    template <typename T>
    struct TypeInfo<T*> : public TypeInfoBase<T>
    {
    };


    /** @brief CTTI for for const pointer types
        @ingroup CTTI
    */
    template <typename T>
    struct TypeInfo<const T*> : public TypeInfoBase<T>
    {
    };



    /** @brief CTTI for for fixed-size array types
        @ingroup CTTI
    */
    template <typename T, std::size_t N>
    struct TypeInfo<T[N]> : public TypeInfoBase<T>
    {
    };


    /** @brief CTTI for for void
        @ingroup CTTI
    */
    template <>
    struct TypeInfo<void>
    {
        typedef void Value;
        typedef void ConstType;
        typedef void Reference;
        typedef void ConstReference;
        typedef void* Pointer;
        typedef void* ConstPointer;
    };

} // !namespace cxxtools


#endif
