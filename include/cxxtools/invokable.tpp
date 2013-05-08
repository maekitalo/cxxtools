// BEGIN_Invokable 10
/**
Invokable is a type which can be "called" via
the invoke() member. It serves as a base type
for other types in the Pt signals/slots framework.
*/
template <class A1 = Void, class A2 = Void, class A3 = Void, class A4 = Void, class A5 = Void, class A6 = Void, class A7 = Void, class A8 = Void, class A9 = Void, class A10 = Void>
class Invokable {
    public:
        /** Does nothing. Does not throw. */
        virtual ~Invokable()
        {}

        /** Behaviour is determined by subclass implementations. */
        virtual void invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10) const = 0;
};
// END_Invokable 10
// BEGIN_Invokable 9
// specialization
template <class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
class Invokable<A1,A2,A3,A4,A5,A6,A7,A8,A9,Void> {
    public:
        /** Does nothing. Does not throw. */
        virtual ~Invokable()
        {}

        /** Behaviour is determined by subclass implementations. */
        virtual void invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9) const = 0;
};
// END_Invokable 9
// BEGIN_Invokable 8
// specialization
template <class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
class Invokable<A1,A2,A3,A4,A5,A6,A7,A8,Void,Void> {
    public:
        /** Does nothing. Does not throw. */
        virtual ~Invokable()
        {}

        /** Behaviour is determined by subclass implementations. */
        virtual void invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8) const = 0;
};
// END_Invokable 8
// BEGIN_Invokable 7
// specialization
template <class A1, class A2, class A3, class A4, class A5, class A6, class A7>
class Invokable<A1,A2,A3,A4,A5,A6,A7,Void,Void,Void> {
    public:
        /** Does nothing. Does not throw. */
        virtual ~Invokable()
        {}

        /** Behaviour is determined by subclass implementations. */
        virtual void invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7) const = 0;
};
// END_Invokable 7
// BEGIN_Invokable 6
// specialization
template <class A1, class A2, class A3, class A4, class A5, class A6>
class Invokable<A1,A2,A3,A4,A5,A6,Void,Void,Void,Void> {
    public:
        /** Does nothing. Does not throw. */
        virtual ~Invokable()
        {}

        /** Behaviour is determined by subclass implementations. */
        virtual void invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) const = 0;
};
// END_Invokable 6
// BEGIN_Invokable 5
// specialization
template <class A1, class A2, class A3, class A4, class A5>
class Invokable<A1,A2,A3,A4,A5,Void,Void,Void,Void,Void> {
    public:
        /** Does nothing. Does not throw. */
        virtual ~Invokable()
        {}

        /** Behaviour is determined by subclass implementations. */
        virtual void invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) const = 0;
};
// END_Invokable 5
// BEGIN_Invokable 4
// specialization
template <class A1, class A2, class A3, class A4>
class Invokable<A1,A2,A3,A4,Void,Void,Void,Void,Void,Void> {
    public:
        /** Does nothing. Does not throw. */
        virtual ~Invokable()
        {}

        /** Behaviour is determined by subclass implementations. */
        virtual void invoke(A1 a1, A2 a2, A3 a3, A4 a4) const = 0;
};
// END_Invokable 4
// BEGIN_Invokable 3
// specialization
template <class A1, class A2, class A3>
class Invokable<A1,A2,A3,Void,Void,Void,Void,Void,Void,Void> {
    public:
        /** Does nothing. Does not throw. */
        virtual ~Invokable()
        {}

        /** Behaviour is determined by subclass implementations. */
        virtual void invoke(A1 a1, A2 a2, A3 a3) const = 0;
};
// END_Invokable 3
// BEGIN_Invokable 2
// specialization
template <class A1, class A2>
class Invokable<A1,A2,Void,Void,Void,Void,Void,Void,Void,Void> {
    public:
        /** Does nothing. Does not throw. */
        virtual ~Invokable()
        {}

        /** Behaviour is determined by subclass implementations. */
        virtual void invoke(A1 a1, A2 a2) const = 0;
};
// END_Invokable 2
// BEGIN_Invokable 1
// specialization
template <class A1>
class Invokable<A1,Void,Void,Void,Void,Void,Void,Void,Void,Void> {
    public:
        /** Does nothing. Does not throw. */
        virtual ~Invokable()
        {}

        /** Behaviour is determined by subclass implementations. */
        virtual void invoke(A1 a1) const = 0;
};
// END_Invokable 1
// BEGIN_Invokable 0
// specialization
template <>
class Invokable<Void,Void,Void,Void,Void,Void,Void,Void,Void,Void> {
    public:
        /** Does nothing. Does not throw. */
        virtual ~Invokable()
        {}

        /** Behaviour is determined by subclass implementations. */
        virtual void invoke() const = 0;
};
// END_Invokable 0
