/*
 * Copyright (C) 2015 Tommi Maekitalo
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

#include "cxxtools/eventloop.h"
#include "cxxtools/unit/testsuite.h"
#include "cxxtools/unit/registertest.h"
#include "cxxtools/event.h"
#include "cxxtools/eventloop.h"

namespace
{
    class TestEvent1 : public cxxtools::BasicEvent<TestEvent1>
    { };

    class TestEvent2 : public cxxtools::BasicEvent<TestEvent2>
    { };
}

class EventLoopTest : public cxxtools::unit::TestSuite
{
    cxxtools::EventLoop _loop;
    std::string _events;

    void onTestEvent1(const TestEvent1&)
    {
        _events += "1";
    }

    void onTestEvent2(const TestEvent2&)
    {
        _events += "2";
    }

public:
    EventLoopTest()
    : cxxtools::unit::TestSuite("eventloop")
    {
        registerMethod("commitEvent", *this, &EventLoopTest::commitEvent);
        registerMethod("priorityEvent", *this, &EventLoopTest::priorityEvent);

        _loop.event.subscribe(slot(*this, &EventLoopTest::onTestEvent1));
        _loop.event.subscribe(slot(*this, &EventLoopTest::onTestEvent2));
    }

    void setUp()
    {
        _events.clear();
    }

    void commitEvent()
    {
        _loop.commitEvent(TestEvent1());
        _loop.commitEvent(TestEvent2());
        _loop.processEvents();
        CXXTOOLS_UNIT_ASSERT_EQUALS(_events, "12");
    }

    void priorityEvent()
    {
        _loop.commitEvent(TestEvent1());
        _loop.commitPriorityEvent(TestEvent2());
        _loop.processEvents();
        CXXTOOLS_UNIT_ASSERT_EQUALS(_events, "21");
    }

};

cxxtools::unit::RegisterTest<EventLoopTest> register_EventLoopTest;
