/*
 * Copyright (C) 2005-2008 by Dr. Marc Boris Duerner
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
        a SourceInfo object. It is recommended to use the CXXTOOLS_UNIT_ASSERT
        for easy creation from a source info object.

        @code
            void myTest()
            {
                int ten = 5 + 5;
                CXXTOOLS_UNIT_ASSERT(ten == 10)
            }
        @endcode
    */
    class Assertion
    {
        public:
            /** @brief Construct from a message and source info.

                Constructs a assertion exception from a message string
                and a source info object that describes where the
                assertion failed. Use the CXXTOOLS_UNIT_ASSERT macro instead
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
        do { \
            if( !(cond) ) \
                throw cxxtools::unit::Assertion(#cond, CXXTOOLS_SOURCEINFO); \
        } while (false)

    #define CXXTOOLS_UNIT_ASSERT_MSG(cond, what) \
        do { \
            if( !(cond) ) \
            { \
                std::ostringstream _cxxtools_msg; \
                _cxxtools_msg << what; \
                throw cxxtools::unit::Assertion(_cxxtools_msg.str(), CXXTOOLS_SOURCEINFO); \
            } \
        } while (false)

    #define CXXTOOLS_UNIT_ASSERT_EQUALS(value1, value2) \
        do { \
            if( ! ((value1) == (value2)) ) \
            { \
                std::ostringstream _cxxtools_msg; \
                _cxxtools_msg << "not equal:\n\tvalue1 (" #value1 ")=\n\t\t<" << (value1) << ">\n\tvalue2 (" #value2 ")=\n\t\t<" << (value2) << '>'; \
                throw cxxtools::unit::Assertion(_cxxtools_msg.str(), CXXTOOLS_SOURCEINFO); \
            } \
        } while (false)

    #define CXXTOOLS_UNIT_ASSERT_THROW(cond, EX) \
        do { \
            struct _cxxtools_ex { }; \
            try \
            { \
                cond; \
                throw _cxxtools_ex(); \
            } \
            catch(const _cxxtools_ex &) \
            { \
                std::ostringstream _cxxtools_msg; \
                _cxxtools_msg << "exception of type " #EX " expected in " #cond; \
                throw cxxtools::unit::Assertion(_cxxtools_msg.str(), CXXTOOLS_SOURCEINFO); \
            } \
            catch(const EX &) \
            {} \
        } while (false)

    #define CXXTOOLS_UNIT_ASSERT_NOTHROW(cond) \
        do { \
            try { \
            \
                cond; \
            } \
            catch(const std::exception& e) \
            { \
                throw cxxtools::unit::Assertion( \
                    std::string("unexpected exception of type ") + typeid(e).name() + ": " + e.what(), \
                    CXXTOOLS_SOURCEINFO); \
            } \
            catch(const cxxtools::unit::Assertion&) \
            { \
                throw; \
            } \
            catch(...) \
            { \
                throw cxxtools::unit::Assertion("unexpected exception." , CXXTOOLS_SOURCEINFO); \
            } \
        } while (false)

    #define CXXTOOLS_UNIT_FAIL(what) \
        do { \
            std::ostringstream _cxxtools_msg; \
            _cxxtools_msg << what; \
            throw cxxtools::unit::Assertion(_cxxtools_msg.str(), CXXTOOLS_SOURCEINFO); \
        } while (false)

} // namespace unit

} // namespace cxxtools

#endif
