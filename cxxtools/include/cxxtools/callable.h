/***************************************************************************
 *   Copyright (C) 2005 by Marc Boris Duerner                              *
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

#ifndef cxxtools_callable_h
#define cxxtools_callable_h

#include <cxxtools/invokable.h>

namespace cxxtools {

    template < typename R,
            typename A1 = Void,
            typename A2 = Void,
            typename A3 = Void,
            typename A4 = Void,
            typename A5 = Void,
            typename A6 = Void,
            typename A7 = Void,
            typename A8 = Void >
    class Callable : public Invokable<A1, A2, A3, A4, A5, A6, A7, A8> {
        public:
            typedef R ReturnT;
            typedef A1 Arg1T;
            typedef A2 Arg2T;
            typedef A3 Arg3T;
            typedef A4 Arg4T;
            typedef A5 Arg5T;
            typedef A6 Arg6T;
            typedef A7 Arg7T;
            typedef A8 Arg8T;
            enum { NumArgs = 8 };

        public:
            virtual Callable* clone() const = 0;

            virtual R operator()(A1, A2, A3, A4, A5, A6, A7, A8) const = 0;

            R call(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8) const
            { return this->operator()(a1, a2, a3, a4, a5, a6, a7, a8); }

            void invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8) const
            { this->operator()(a1, a2, a3, a4, a5, a6, a7, a8); }
    };


    template < typename A1,
            typename A2,
            typename A3,
            typename A4,
            typename A5,
            typename A6,
            typename A7,
            typename A8 >
    class Callable<void, A1, A2, A3, A4, A5, A6, A7, A8> : public Invokable<A1, A2, A3, A4, A5, A6, A7, A8>{
        public:
            typedef void ReturnT;
            typedef A1 Arg1T;
            typedef A2 Arg2T;
            typedef A3 Arg3T;
            typedef A4 Arg4T;
            typedef A5 Arg5T;
            typedef A6 Arg6T;
            typedef A7 Arg7T;
            typedef A8 Arg8T;
            enum { NumArgs = 8 };

        public:
            virtual Callable* clone() const = 0;

            virtual void operator()(A1, A2, A3, A4, A5, A6, A7, A8) const = 0;

            void call(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8) const
            { return this->operator()(a1, a2, a3, a4, a5, a6, a7, a8); }

            void invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8) const
            { this->operator()(a1, a2, a3, a4, a5, a6, a7, a8); }
    };


    template < typename R,
            typename A1,
            typename A2,
            typename A3,
            typename A4,
            typename A5>
    class Callable<R, A1, A2, A3, A4, A5, Void, Void, Void> : public Invokable<A1, A2, A3, A4, A5> {
        public:
            typedef R ReturnT;
            typedef A1 Arg1T;
            typedef A2 Arg2T;
            typedef A3 Arg3T;
            typedef A4 Arg4T;
            typedef A5 Arg5T;
            enum { NumArgs = 5 };

        public:
            virtual Callable* clone() const = 0;

            virtual R operator()(A1, A2, A3, A4, A5) const = 0;

            R call(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) const
            { return this->operator()(a1, a2, a3, a4, a5); }

            void invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) const
            { this->operator()(a1, a2, a3, a4, a5); }
    };


    template < typename A1,
            typename A2,
            typename A3,
            typename A4,
            typename A5 >
    class Callable<void, A1, A2, A3, A4, A5, Void, Void, Void> : public Invokable<A1, A2, A3, A4, A5>{
        public:
            typedef void ReturnT;
            typedef A1 Arg1T;
            typedef A2 Arg2T;
            typedef A3 Arg3T;
            typedef A4 Arg4T;
            typedef A5 Arg5T;
            enum { NumArgs = 5 };

        public:
            virtual Callable* clone() const = 0;

            virtual void operator()(A1, A2, A3, A4, A5) const = 0;

            void call(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) const
            { return this->operator()(a1, a2, a3, a4, a5); }

            void invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) const
            { this->operator()(a1, a2, a3, a4, a5); }
    };


    template < typename R,
            typename A1,
            typename A2,
            typename A3,
            typename A4 >
    class Callable<R, A1, A2, A3, A4, Void, Void, Void, Void> : public Invokable<A1, A2, A3, A4> {
        public:
            typedef R ReturnT;
            typedef A1 Arg1T;
            typedef A2 Arg2T;
            typedef A3 Arg3T;
            typedef A4 Arg4T;
            enum { NumArgs = 4 };

        public:
            virtual Callable* clone() const = 0;

            virtual R operator()(A1, A2, A3, A4) const = 0;

            R call(A1 a1, A2 a2, A3 a3, A4 a4) const
            { return this->operator()(a1, a2, a3, a4); }

            void invoke(A1 a1, A2 a2, A3 a3, A4 a4) const
            { this->operator()(a1, a2, a3, a4); }
    };


    template < typename A1,
                typename A2,
                typename A3,
                typename A4 >
    class Callable<void, A1, A2, A3, A4, Void, Void, Void, Void> : public Invokable<A1, A2, A3, A4>{
        public:
            typedef void ReturnT;
            typedef A1 Arg1T;
            typedef A2 Arg2T;
            typedef A3 Arg3T;
            typedef A4 Arg4T;
            enum { NumArgs = 4 };

        public:
            virtual Callable* clone() const = 0;

            virtual void operator()(A1, A2, A3, A4) const = 0;

            void call(A1 a1, A2 a2, A3 a3, A4 a4) const
            { return this->operator()(a1, a2, a3, a4); }

            void invoke(A1 a1, A2 a2, A3 a3, A4 a4) const
            { this->operator()(a1, a2, a3, a4); }
    };


    template < typename R,
                typename A1,
                typename A2,
                typename A3 >
    class Callable<R, A1, A2, A3, Void, Void, Void, Void, Void> : public Invokable<A1, A2, A3> {
        public:
            typedef R ReturnT;
            typedef A1 Arg1T;
            typedef A2 Arg2T;
            typedef A3 Arg3T;
            enum { NumArgs = 3 };

        public:
            virtual Callable* clone() const = 0;

            virtual R operator()(A1, A2, A3) const = 0;

            R call(A1 a1, A2 a2, A3 a3) const
            { return this->operator()(a1, a2, a3); }

            void invoke(A1 a1, A2 a2, A3 a3) const
            { this->operator()(a1, a2, a3); }
    };


    template < typename A1,
                typename A2,
                typename A3 >
    class Callable<void, A1, A2, A3, Void, Void, Void, Void, Void> : public Invokable<A1, A2, A3> {
        public:
            typedef void ReturnT;
            typedef A1 Arg1T;
            typedef A2 Arg2T;
            typedef A3 Arg3T;
            enum { NumArgs = 3 };

        public:
            virtual Callable* clone() const = 0;

            virtual void operator()(A1, A2, A3) const = 0;

            void call(A1 a1, A2 a2, A3 a3) const
            { return this->operator()(a1, a2, a3); }

            void invoke(A1 a1, A2 a2, A3 a3) const
            { this->operator()(a1, a2, a3); }
    };


    template < typename R,
                typename A1,
                typename A2 >
    class Callable<R, A1, A2, Void, Void, Void, Void, Void, Void> : public Invokable<A1, A2> {
        public:
            typedef R ReturnT;
            typedef A1 Arg1T;
            typedef A2 Arg2T;
            typedef Void Arg3T;
            enum { NumArgs = 2 };

        public:
            virtual Callable* clone() const = 0;

            virtual R operator()(A1, A2) const = 0;

            R call(A1 a1, A2 a2) const
            { return this->operator()(a1, a2); }

            void invoke(A1 a1, A2 a2) const
            { this->operator()(a1, a2); }
    };


    template < typename A1,
            typename A2 >
    class Callable<void, A1, A2, Void, Void, Void, Void, Void, Void> : public Invokable<A1, A2> {
        public:
            typedef void ReturnT;
            typedef A1 Arg1T;
            typedef A2 Arg2T;
            typedef Void Arg3T;
            enum { NumArgs = 2 };

        public:
            virtual Callable* clone() const = 0;

            virtual void operator()(A1, A2) const = 0;

            void call(A1 a1, A2 a2) const
            { return this->operator()(a1, a2); }

            void invoke(A1 a1, A2 a2) const
            { this->operator()(a1, a2); }
    };


    template < typename R,
            typename A1 >
    class Callable<R,
                A1,
                Void,
                Void,
                Void,
                Void,
                Void,
                Void,
                Void> : public Invokable<A1> {
        public:
            typedef R ReturnT;
            typedef A1 Arg1T;
            typedef Void Arg2T;
            typedef Void Arg3T;
            enum { NumArgs = 1 };

        public:
            virtual Callable* clone() const = 0;

            virtual R operator()(A1) const = 0;

            R call(A1 a1) const
            { return this->operator()(a1); }

            void invoke(A1 a1) const
            { this->operator()(a1); }
    };


    template <typename A1>
    class Callable<void,
                A1,
                Void,
                Void,
                Void,
                Void,
                Void,
                Void,
                Void> : public Invokable<A1> {
        public:
            typedef void ReturnT;
            typedef A1 Arg1T;
            typedef Void Arg2T;
            typedef Void Arg3T;
            enum { NumArgs = 1 };

        public:
            virtual Callable* clone() const = 0;

            virtual void operator()(A1) const = 0;

            void call(A1 a1) const
            { return this->operator()(a1); }

            void invoke(A1 a1) const
            { this->operator()(a1); }
    };


    template <typename R>
    class Callable<R,
                Void,
                Void,
                Void,
                Void,
                Void,
                Void,
                Void,
                Void> : public Invokable<> {
        public:
            typedef R ReturnT;
            typedef Void Arg1T;
            typedef Void Arg2T;
            typedef Void Arg3T;
            enum { NumArgs = 0 };

        public:
            virtual Callable* clone() const = 0;

            virtual R operator()() const = 0;

            R call() const
            { return this->operator()(); }

            void invoke() const
            { this->operator()(); }
    };


    template <>
    class Callable<void,
                   Void,
                   Void,
                   Void,
                   Void,
                   Void,
                   Void,
                   Void,
                   Void> : public Invokable<> {
        public:
            typedef void ReturnT;
            typedef Void Arg1T;
            typedef Void Arg2T;
            typedef Void Arg3T;
            enum { NumArgs = 0 };

        public:
            virtual Callable* clone() const = 0;

            virtual void operator()() const = 0;

            void call() const
            { this->operator()(); }

            void invoke() const
            { this->operator()(); }
    };

} // namespace cxxtools

#endif
