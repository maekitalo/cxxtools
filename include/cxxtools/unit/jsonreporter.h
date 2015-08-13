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

#ifndef CXXTOOLS_UNIT_JSONREPORTER_H
#define CXXTOOLS_UNIT_JSONREPORTER_H

#include <cxxtools/unit/reporter.h>
#include <cxxtools/serializationinfo.h>

namespace cxxtools {

namespace unit {


/** This class collects test results and makes them serialziable.

    The name of the class is somewhat misleading since it can be used as well
    with xml serializer.

    To use it, attach the reporter to the test application. When the tests were
    run, output the reporter with a suitable serializer like the json
    serializer.

    \code

      // instantiate the test application
      cxxtools::unit::Application app;

      // create a JsonReporter and attach it to the app
      JsonReporter reporter;
      app.attachReporter(reporter);

      // run all registered tests
      app.run();

      // output the results in json format
      std::cout << cxxtools::Json(reporter).beautify(true);

    \endcode
 */
class JsonReporter : public cxxtools::unit::Reporter
{
    friend void operator <<= (cxxtools::SerializationInfo& si, const JsonReporter& result);

    cxxtools::SerializationInfo si;
    cxxtools::SerializationInfo* current;

public:
    JsonReporter()
        : current(0)
        { }

    virtual void reportStart(const cxxtools::unit::TestContext& test);
    virtual void reportFinish(const cxxtools::unit::TestContext& test);
    virtual void reportMessage(const std::string& msg);
    virtual void reportSuccess(const cxxtools::unit::TestContext& test);
    virtual void reportAssertion(const cxxtools::unit::TestContext& test, const cxxtools::unit::Assertion& a);
    virtual void reportException(const cxxtools::unit::TestContext& test, const std::exception& ex);
    virtual void reportError(const cxxtools::unit::TestContext& test);
    virtual void reportSkip(const cxxtools::unit::TestContext& test);
};

}
}

#endif
