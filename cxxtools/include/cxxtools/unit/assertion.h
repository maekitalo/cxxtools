/***************************************************************************
 *   Copyright (C) 2005-2008 by Dr. Marc Boris Duerner                     *
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
#ifndef CXXTOOLS_ASSERTION_H
#define CXXTOOLS_ASSERTION_H

#include "cxxtools/sourceinfo.h"
#include <stdexcept>
#include <iostream>
#include <sstream>

namespace cxxtools {

namespace unit {

    /** @brief %Test %Assertion exception
        @ingroup UnitTests

        Assertions are modeled as an exception type, which is thrown by Unit
        tests when an assertion has failed. This class implements
        std::exception and overrides std::exception::what() to return an
        error message Besides the error message, Assertions can provide
        information where the exception was raised in the source code through
        a SourceInfo object. It is recommended to use the PT_UNIT_ASSERT
        for easy creation from a source info object.

        @code
            void myTest()
            {
                int ten = 5 + 5;
                PT_UNIT_ASSERT(ten == 10)
            }
        @endcode
    */
    class Assertion
    {
        public:
            /** @brief Construct from a message and source info.

                Constructs a assertion exception from a message string
                and a source info object that describes where the
                assertion failed. Use the PT_UNIT_ASSERT macro instead
                of this constructor.

                @param what Error message
                @param si Info where the assertion failed
            */
            Assertion(const std::string& what, const SourceInfo& si);

            const SourceInfo& sourceInfo() const;

            const char* what() const  { return _what.c_str(); }

        private:
            SourceInfo _sourceInfo;
            std::string _what;
    };

    #define CXXTOOLS_UNIT_ASSERT(cond) \
        if( !(cond) ) throw cxxtools::unit::Assertion(#cond, CXXTOOLS_SOURCEINFO);

    #define CXXTOOLS_UNIT_ASSERT_MSG(cond, what) \
        if( !(cond) ) throw cxxtools::unit::Assertion((what), CXXTOOLS_SOURCEINFO);

    #define CXXTOOLS_UNIT_ASSERT_EQUALS(value1, value2) \
        if( ! ((value1) == (value2)) ) \
        { \
          std::ostringstream _cxxtools_msg; \
          _cxxtools_msg << "not equal: value1 (" #value1 ")=<" << value1 << "> value2 (" #value2 ")=<" << value2 << '>'; \
          throw cxxtools::unit::Assertion(_cxxtools_msg.str(), CXXTOOLS_SOURCEINFO); \
        }

    #define CXXTOOLS_UNIT_ASSERT_THROW(cond, EX) \
        try { \
            cond; \
            throw std::string("Exception expected."); \
        } \
        catch(const std::string & s) \
        { \
            throw cxxtools::unit::Assertion(s, CXXTOOLS_SOURCEINFO); \
        } \
        catch(const EX &) \
        {} \

} // namespace unit

} // namespace cxxtools

#endif
