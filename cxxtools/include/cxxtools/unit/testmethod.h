/***************************************************************************
 *   Copyright (C) 2005-2006 by Dr. Marc Boris Duerner                     *
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

} // namespace unit

} // namespace cxxtools

#endif // for header

