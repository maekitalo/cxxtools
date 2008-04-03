/***************************************************************************
 *   Copyright (C) 2005-2008 by Dr. Marc Boris Duerner                      *
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
#ifndef CXXTOOLS_UNIT_API_H
#define CXXTOOLS_UNIT_API_H

#include <cxxtools/api.h>

#if defined(CXXTOOLS_UNIT_API_EXPORT)
#    define CXXTOOLS_UNIT_API CXXTOOLS_EXPORT
#  else
#    define CXXTOOLS_UNIT_API CXXTOOLS_IMPORT
#  endif

#endif

/** @defgroup UnitTests UnitTests
    @brief Protocol and data driven Unit-testing

    This module provides a complete framework for effective Unit testing. All Unit
    tests in the Platinum framework are written using this module. Data-driven,
    as well as and protocol-driven testing is possible. %Unit tests can easily be
    integrated into the build process and test results can be reported and logged.
    Writing tests is similar to writing tests with the CppUnit framework, so that
    porting tests to use %Pt::Unit is simple.
*/

namespace cxxtools {

/** @namespace Pt::Unit
    @brief Protocol and data driven Unit-testing

    This module provides a complete framework for effective Unit testing. All Unit
    tests in the Platinum framework are written using this module. Data-driven,
    as well as and protocol-driven testing is possible. %Unit tests can easily be
    integrated into the build process and test results can be reported and logged.
    Writing tests is similar to writing tests with the CppUnit framework, so that
    porting tests to use %Pt::Unit is simple.
*/
namespace unit {

    class Application;
    class Assertion;
    class Reporter;
    class Test;
    class TestCase;
    class TestContext;
    class TestFixture;
    class TestProtocol;
    class TextProtocol;
    class TestSuite;

}

}
