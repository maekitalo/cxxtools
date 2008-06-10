/***************************************************************************
 *   Copyright (C) 2005-2008 by Marc Boris Duerner                         *
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
#ifndef CXXTOOLS_UNIT_TESTCONTEXT_H
#define CXXTOOLS_UNIT_TESTCONTEXT_H

#include <string>
#include <cstddef>

namespace cxxtools {

namespace unit {

    class Test;
    class TestFixture;

    class TestContext
    {
        public:
            virtual ~TestContext();

            std::string testName() const;

            void run();

        protected:
            TestContext(TestFixture& fixture, Test& test);

            virtual void exec() = 0;

        private:
            TestFixture& _fixture;
            Test& _test;
            bool _setUp;
    };

} // namespace unit

} // namespace cxxtools

#endif
