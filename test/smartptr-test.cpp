/*
 * Copyright (C) 2006 by Tommi Maekitalo
 * Copyright (C) 2006 by Marc Boris Duerner
 * Copyright (C) 2006 by Stefan Bueder
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

#include "cxxtools/smartptr.h"
#include "cxxtools/refcounted.h"
#include "cxxtools/unit/assertion.h"
#include "cxxtools/unit/testsuite.h"
#include "cxxtools/unit/registertest.h"


class Object : public cxxtools::RefCounted
{
    public:
        Object()
        { ++objectRefs; }

        ~Object()
        { --objectRefs; }

        static std::size_t objectRefs;
};

std::size_t Object::objectRefs = 0;

class AtomicObject : public cxxtools::AtomicRefCounted
{
    public:
        AtomicObject()
        { ++objectRefs; }

        ~AtomicObject()
        { --objectRefs; }

        static std::size_t objectRefs;
};

std::size_t AtomicObject::objectRefs = 0;


class SmartPtrTest : public cxxtools::unit::TestSuite
{
    public:
        SmartPtrTest()
        : cxxtools::unit::TestSuite( "SmartPtr" )
        {
            registerMethod( "RefCounted", *this, &SmartPtrTest::RefCounted );
            registerMethod( "InternalRefCounted", *this, &SmartPtrTest::InternalRefCounted );
            registerMethod( "AtomicInternalRefCounted", *this, &SmartPtrTest::AtomicInternalRefCounted );
            registerMethod( "RefLinked", *this, &SmartPtrTest::RefLinked );
        }

    public:
        void setUp();

    protected:
        void RefCounted();
        void InternalRefCounted();
        void AtomicInternalRefCounted();
        void RefLinked();
};

cxxtools::unit::RegisterTest<SmartPtrTest> register_SmartPtrTest;


void SmartPtrTest::setUp()
{
    Object::objectRefs = 0;
    AtomicObject::objectRefs = 0;
}


void SmartPtrTest::RefCounted()
{
    Object* obj = new Object();

    typedef cxxtools::SmartPtr<Object, cxxtools::ExternalRefCounted> Ptr;

    {
        Ptr smartPtr(obj);
        CXXTOOLS_UNIT_ASSERT_EQUALS( smartPtr.refs(), 1 );

        Ptr second(smartPtr);
        CXXTOOLS_UNIT_ASSERT_EQUALS( second.refs(), 2);

        Ptr third;
        third = second;
        CXXTOOLS_UNIT_ASSERT_EQUALS( third.refs(), 3);

        third = third;
        CXXTOOLS_UNIT_ASSERT_EQUALS( third.refs(), 3);
    }

    CXXTOOLS_UNIT_ASSERT_EQUALS(Object::objectRefs, 0);
}


void SmartPtrTest::InternalRefCounted()
{
    Object* obj = new Object();

    typedef cxxtools::SmartPtr<Object, cxxtools::InternalRefCounted> Ptr;

    {
        Ptr smartPtr(obj);
        CXXTOOLS_UNIT_ASSERT_EQUALS( smartPtr->refs(), 1 );

        Ptr second(smartPtr);
        CXXTOOLS_UNIT_ASSERT_EQUALS( second->refs(), 2);

        Ptr third;
        third = second;
        CXXTOOLS_UNIT_ASSERT_EQUALS( third->refs(), 3);

        third = third;
        CXXTOOLS_UNIT_ASSERT_EQUALS( third->refs(), 3);
    }

    CXXTOOLS_UNIT_ASSERT_EQUALS(Object::objectRefs, 0);
}


void SmartPtrTest::AtomicInternalRefCounted()
{
    AtomicObject* obj = new AtomicObject();

    typedef cxxtools::SmartPtr<AtomicObject, cxxtools::InternalRefCounted> Ptr;

    {
        Ptr smartPtr(obj);
        CXXTOOLS_UNIT_ASSERT_EQUALS( smartPtr->refs(), 1 );

        Ptr second(smartPtr);
        CXXTOOLS_UNIT_ASSERT_EQUALS( second->refs(), 2);

        Ptr third;
        third = second;
        CXXTOOLS_UNIT_ASSERT_EQUALS( third->refs(), 3);

        third = third;
        CXXTOOLS_UNIT_ASSERT_EQUALS( third->refs(), 3);
    }

    CXXTOOLS_UNIT_ASSERT_EQUALS(AtomicObject::objectRefs, 0);
}


void SmartPtrTest::RefLinked()
{
    Object* obj = new Object();

    typedef cxxtools::SmartPtr<Object, cxxtools::RefLinked> Ptr;

    {
        Ptr smartPtr(obj);
        Ptr second(smartPtr);
        Ptr third;
        Ptr fourth(third);

        third = second;
    }

    CXXTOOLS_UNIT_ASSERT(Object::objectRefs == 0);
}

