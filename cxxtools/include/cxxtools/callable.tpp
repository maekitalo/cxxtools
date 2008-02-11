// BEGIN_Callable 10
    /**
      Callable represents a "callable" type, which is fundamentally
      similar to an Invokable but is semantically richer.
    */
    template <typename R, class A1 = Void, class A2 = Void, class A3 = Void, class A4 = Void, class A5 = Void, class A6 = Void, class A7 = Void, class A8 = Void, class A9 = Void, class A10 = Void>
    class Callable : public Invokable<A1,A2,A3,A4,A5,A6,A7,A8,A9,A10> {
		public:
            typedef R ReturnT;
            enum { NumArgs = 10 };

        public:
            /**
              Creates a copy of this object and returns it. Caller owns
              the returned object.
            */
            virtual Callable* clone() const = 0;

            /** Exact behaviour is defined by subclass implementations. */
            virtual ReturnT operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10) const = 0;

            /** Same as calling this->operator()(...). */
            ReturnT call(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10) const
            { return this->operator()(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10); }

            /** Same as calling this->operator()(...), except that the return value
            of that method is ignored. */
            void invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10) const
            { this->operator()(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10); }
    };

// END_Callable 10
// BEGIN_Callable 9
    // specialization
    template <typename R,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
    class Callable<R, A1,A2,A3,A4,A5,A6,A7,A8,A9,Void> : public Invokable<A1,A2,A3,A4,A5,A6,A7,A8,A9> {
		public:
            typedef R ReturnT;
            enum { NumArgs = 9 };

        public:
            /**
              Creates a copy of this object and returns it. Caller owns
              the returned object.
            */
            virtual Callable* clone() const = 0;

            /** Exact behaviour is defined by subclass implementations. */
            virtual ReturnT operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9) const = 0;

            /** Same as calling this->operator()(...). */
            ReturnT call(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9) const
            { return this->operator()(a1,a2,a3,a4,a5,a6,a7,a8,a9); }

            /** Same as calling this->operator()(...), except that the return value
            of that method is ignored. */
            void invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9) const
            { this->operator()(a1,a2,a3,a4,a5,a6,a7,a8,a9); }
    };

// END_Callable 9
// BEGIN_Callable 8
    // specialization
    template <typename R,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
    class Callable<R, A1,A2,A3,A4,A5,A6,A7,A8,Void,Void> : public Invokable<A1,A2,A3,A4,A5,A6,A7,A8> {
		public:
            typedef R ReturnT;
            enum { NumArgs = 8 };

        public:
            /**
              Creates a copy of this object and returns it. Caller owns
              the returned object.
            */
            virtual Callable* clone() const = 0;

            /** Exact behaviour is defined by subclass implementations. */
            virtual ReturnT operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8) const = 0;

            /** Same as calling this->operator()(...). */
            ReturnT call(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8) const
            { return this->operator()(a1,a2,a3,a4,a5,a6,a7,a8); }

            /** Same as calling this->operator()(...), except that the return value
            of that method is ignored. */
            void invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8) const
            { this->operator()(a1,a2,a3,a4,a5,a6,a7,a8); }
    };

// END_Callable 8
// BEGIN_Callable 7
    // specialization
    template <typename R,class A1, class A2, class A3, class A4, class A5, class A6, class A7>
    class Callable<R, A1,A2,A3,A4,A5,A6,A7,Void,Void,Void> : public Invokable<A1,A2,A3,A4,A5,A6,A7> {
		public:
            typedef R ReturnT;
            enum { NumArgs = 7 };

        public:
            /**
              Creates a copy of this object and returns it. Caller owns
              the returned object.
            */
            virtual Callable* clone() const = 0;

            /** Exact behaviour is defined by subclass implementations. */
            virtual ReturnT operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7) const = 0;

            /** Same as calling this->operator()(...). */
            ReturnT call(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7) const
            { return this->operator()(a1,a2,a3,a4,a5,a6,a7); }

            /** Same as calling this->operator()(...), except that the return value
            of that method is ignored. */
            void invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7) const
            { this->operator()(a1,a2,a3,a4,a5,a6,a7); }
    };

// END_Callable 7
// BEGIN_Callable 6
    // specialization
    template <typename R,class A1, class A2, class A3, class A4, class A5, class A6>
    class Callable<R, A1,A2,A3,A4,A5,A6,Void,Void,Void,Void> : public Invokable<A1,A2,A3,A4,A5,A6> {
		public:
            typedef R ReturnT;
            enum { NumArgs = 6 };

        public:
            /**
              Creates a copy of this object and returns it. Caller owns
              the returned object.
            */
            virtual Callable* clone() const = 0;

            /** Exact behaviour is defined by subclass implementations. */
            virtual ReturnT operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) const = 0;

            /** Same as calling this->operator()(...). */
            ReturnT call(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) const
            { return this->operator()(a1,a2,a3,a4,a5,a6); }

            /** Same as calling this->operator()(...), except that the return value
            of that method is ignored. */
            void invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) const
            { this->operator()(a1,a2,a3,a4,a5,a6); }
    };

// END_Callable 6
// BEGIN_Callable 5
    // specialization
    template <typename R,class A1, class A2, class A3, class A4, class A5>
    class Callable<R, A1,A2,A3,A4,A5,Void,Void,Void,Void,Void> : public Invokable<A1,A2,A3,A4,A5> {
		public:
            typedef R ReturnT;
            enum { NumArgs = 5 };

        public:
            /**
              Creates a copy of this object and returns it. Caller owns
              the returned object.
            */
            virtual Callable* clone() const = 0;

            /** Exact behaviour is defined by subclass implementations. */
            virtual ReturnT operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) const = 0;

            /** Same as calling this->operator()(...). */
            ReturnT call(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) const
            { return this->operator()(a1,a2,a3,a4,a5); }

            /** Same as calling this->operator()(...), except that the return value
            of that method is ignored. */
            void invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) const
            { this->operator()(a1,a2,a3,a4,a5); }
    };

// END_Callable 5
// BEGIN_Callable 4
    // specialization
    template <typename R,class A1, class A2, class A3, class A4>
    class Callable<R, A1,A2,A3,A4,Void,Void,Void,Void,Void,Void> : public Invokable<A1,A2,A3,A4> {
		public:
            typedef R ReturnT;
            enum { NumArgs = 4 };

        public:
            /**
              Creates a copy of this object and returns it. Caller owns
              the returned object.
            */
            virtual Callable* clone() const = 0;

            /** Exact behaviour is defined by subclass implementations. */
            virtual ReturnT operator()(A1 a1, A2 a2, A3 a3, A4 a4) const = 0;

            /** Same as calling this->operator()(...). */
            ReturnT call(A1 a1, A2 a2, A3 a3, A4 a4) const
            { return this->operator()(a1,a2,a3,a4); }

            /** Same as calling this->operator()(...), except that the return value
            of that method is ignored. */
            void invoke(A1 a1, A2 a2, A3 a3, A4 a4) const
            { this->operator()(a1,a2,a3,a4); }
    };

// END_Callable 4
// BEGIN_Callable 3
    // specialization
    template <typename R,class A1, class A2, class A3>
    class Callable<R, A1,A2,A3,Void,Void,Void,Void,Void,Void,Void> : public Invokable<A1,A2,A3> {
		public:
            typedef R ReturnT;
            enum { NumArgs = 3 };

        public:
            /**
              Creates a copy of this object and returns it. Caller owns
              the returned object.
            */
            virtual Callable* clone() const = 0;

            /** Exact behaviour is defined by subclass implementations. */
            virtual ReturnT operator()(A1 a1, A2 a2, A3 a3) const = 0;

            /** Same as calling this->operator()(...). */
            ReturnT call(A1 a1, A2 a2, A3 a3) const
            { return this->operator()(a1,a2,a3); }

            /** Same as calling this->operator()(...), except that the return value
            of that method is ignored. */
            void invoke(A1 a1, A2 a2, A3 a3) const
            { this->operator()(a1,a2,a3); }
    };

// END_Callable 3
// BEGIN_Callable 2
    // specialization
    template <typename R,class A1, class A2>
    class Callable<R, A1,A2,Void,Void,Void,Void,Void,Void,Void,Void> : public Invokable<A1,A2> {
		public:
            typedef R ReturnT;
            enum { NumArgs = 2 };

        public:
            /**
              Creates a copy of this object and returns it. Caller owns
              the returned object.
            */
            virtual Callable* clone() const = 0;

            /** Exact behaviour is defined by subclass implementations. */
            virtual ReturnT operator()(A1 a1, A2 a2) const = 0;

            /** Same as calling this->operator()(...). */
            ReturnT call(A1 a1, A2 a2) const
            { return this->operator()(a1,a2); }

            /** Same as calling this->operator()(...), except that the return value
            of that method is ignored. */
            void invoke(A1 a1, A2 a2) const
            { this->operator()(a1,a2); }
    };

// END_Callable 2
// BEGIN_Callable 1
    // specialization
    template <typename R,class A1>
    class Callable<R, A1,Void,Void,Void,Void,Void,Void,Void,Void,Void> : public Invokable<A1> {
		public:
            typedef R ReturnT;
            enum { NumArgs = 1 };

        public:
            /**
              Creates a copy of this object and returns it. Caller owns
              the returned object.
            */
            virtual Callable* clone() const = 0;

            /** Exact behaviour is defined by subclass implementations. */
            virtual ReturnT operator()(A1 a1) const = 0;

            /** Same as calling this->operator()(...). */
            ReturnT call(A1 a1) const
            { return this->operator()(a1); }

            /** Same as calling this->operator()(...), except that the return value
            of that method is ignored. */
            void invoke(A1 a1) const
            { this->operator()(a1); }
    };

// END_Callable 1
// BEGIN_Callable 0
    // specialization
    template <typename R>
    class Callable<R, Void,Void,Void,Void,Void,Void,Void,Void,Void,Void> : public Invokable<> {
		public:
            typedef R ReturnT;
            enum { NumArgs = 0 };

        public:
            /**
              Creates a copy of this object and returns it. Caller owns
              the returned object.
            */
            virtual Callable* clone() const = 0;

            /** Exact behaviour is defined by subclass implementations. */
            virtual ReturnT operator()() const = 0;

            /** Same as calling this->operator()(...). */
            ReturnT call() const
            { return this->operator()(); }

            /** Same as calling this->operator()(...), except that the return value
            of that method is ignored. */
            void invoke() const
            { this->operator()(); }
    };

// END_Callable 0
