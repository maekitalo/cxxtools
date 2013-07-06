/*
 * Copyright (C) 2005-2006 by Dr. Marc Boris Duerner
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
#ifndef CXXTOOLS_UNIT_TESTMETHOD_H
#define CXXTOOLS_UNIT_TESTMETHOD_H

#include <cxxtools/unit/test.h>
#include <stdexcept>
#include <cstddef>

namespace cxxtools {

    class SerializationInfo;

namespace unit {

    class TestMethod : public cxxtools::unit::Test
    {
        public:
            TestMethod(const std::string& name)
            : cxxtools::unit::Test(name)
            {}

            virtual ~TestMethod()
            {}

            virtual void run()
            {}

            virtual void exec(const SerializationInfo* si, unsigned argCount) = 0;
    };


    //! @cond internal

    template < class C,
               typename A1 = cxxtools::Void,
               typename A2 = cxxtools::Void,
               typename A3 = cxxtools::Void,
               typename A4 = cxxtools::Void,
               typename A5 = cxxtools::Void,
               typename A6 = cxxtools::Void,
               typename A7 = cxxtools::Void,
               typename A8 = cxxtools::Void >
    class BasicTestMethod : public cxxtools::Method<void, C, A1, A2, A3, A4, A5, A6, A7, A8>
                          , public TestMethod
    {
        public:
            typedef C ClassT;
            typedef void (C::*MemFuncT)(A1, A2, A3, A4, A5, A6, A7, A8);

        public:
            BasicTestMethod(const std::string& name, C& object, MemFuncT ptr)
            : cxxtools::Method<void, C, A1, A2, A3, A4, A5, A6, A7, A8>(object, ptr)
            , TestMethod(name)
            {}

            void exec(const SerializationInfo* args, unsigned argCount)
            {
                throw std::logic_error("SerializationInfo not implemented");
            }

            virtual void run()
            {}
    };


    template < class C >
    class BasicTestMethod<C,
                          cxxtools::Void,
                          cxxtools::Void,
                          cxxtools::Void,
                          cxxtools::Void,
                          cxxtools::Void,
                          cxxtools::Void,
                          cxxtools::Void,
                          cxxtools::Void> : public cxxtools::Method<void, C>
                                    , public TestMethod
    {
        public:
            typedef C ClassT;
            typedef void (C::*MemFuncT)();

        public:
            BasicTestMethod(const std::string& name, C& object, MemFuncT ptr)
            : cxxtools::Method<void, C>(object, ptr)
            , TestMethod(name)
            {}

            void exec(const SerializationInfo* si, unsigned argCount)
            {
                cxxtools::Method<void, C>::call();
            }

            virtual void run()
            {}
    };

    //! @endcond internal

} // namespace unit

} // namespace cxxtools

#endif // for header

