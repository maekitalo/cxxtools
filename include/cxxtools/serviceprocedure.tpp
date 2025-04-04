// generated with `serviceprocedure.pl -n 10`
//! @cond internal
// BasicServiceProcedure with 10 arguments
template <typename R,
          typename A1 = Void,
          typename A2 = Void,
          typename A3 = Void,
          typename A4 = Void,
          typename A5 = Void,
          typename A6 = Void,
          typename A7 = Void,
          typename A8 = Void,
          typename A9 = Void,
          typename A10 = Void>
class BasicServiceProcedure : public ServiceProcedure
{
    public:
        explicit BasicServiceProcedure(const Callable<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>& cb)
        : ServiceProcedure(),
          _cb(0),
          _composers(_args, 10)
        {
            _cb = cb.clone();

            _args[0] = &_a1;
            _args[1] = &_a2;
            _args[2] = &_a3;
            _args[3] = &_a4;
            _args[4] = &_a5;
            _args[5] = &_a6;
            _args[6] = &_a7;
            _args[7] = &_a8;
            _args[8] = &_a9;
            _args[9] = &_a10;
        }

        ~BasicServiceProcedure()
        {
            delete _cb;
        }

        ServiceProcedure* clone() const
        {
            return new BasicServiceProcedure(*_cb);
        }

        IComposers* beginCall()
        {
            _a1.begin(_v1);
            _a2.begin(_v2);
            _a3.begin(_v3);
            _a4.begin(_v4);
            _a5.begin(_v5);
            _a6.begin(_v6);
            _a7.begin(_v7);
            _a8.begin(_v8);
            _a9.begin(_v9);
            _a10.begin(_v10);

            return &_composers;
        }

        IDecomposer* endCall()
        {
            _rv = _cb->call(_v1, _v2, _v3, _v4, _v5, _v6, _v7, _v8, _v9, _v10);
            _r.begin(_rv);
            return &_r;
        }

    private:
        typedef typename TypeTraits<A1>::Value V1;
        typedef typename TypeTraits<A2>::Value V2;
        typedef typename TypeTraits<A3>::Value V3;
        typedef typename TypeTraits<A4>::Value V4;
        typedef typename TypeTraits<A5>::Value V5;
        typedef typename TypeTraits<A6>::Value V6;
        typedef typename TypeTraits<A7>::Value V7;
        typedef typename TypeTraits<A8>::Value V8;
        typedef typename TypeTraits<A9>::Value V9;
        typedef typename TypeTraits<A10>::Value V10;
        typedef typename TypeTraits<R>::Value RV;

        Callable<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>* _cb;
        RV _rv;
        V1 _v1;
        V2 _v2;
        V3 _v3;
        V4 _v4;
        V5 _v5;
        V6 _v6;
        V7 _v7;
        V8 _v8;
        V9 _v9;
        V10 _v10;

        IComposer* _args[10];
        Composers _composers;
        Composer<V1> _a1;
        Composer<V2> _a2;
        Composer<V3> _a3;
        Composer<V4> _a4;
        Composer<V5> _a5;
        Composer<V6> _a6;
        Composer<V7> _a7;
        Composer<V8> _a8;
        Composer<V9> _a9;
        Composer<V10> _a10;
        Decomposer<RV> _r;
};


// BasicServiceProcedure with 9 arguments
template <typename R,
          typename A1,
          typename A2,
          typename A3,
          typename A4,
          typename A5,
          typename A6,
          typename A7,
          typename A8,
          typename A9>
class BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7, A8, A9,
                            Void> : public ServiceProcedure
{
    public:
        explicit BasicServiceProcedure(const Callable<R, A1, A2, A3, A4, A5, A6, A7, A8, A9>& cb)
        : ServiceProcedure(),
          _cb(0),
          _composers(_args, 9)
        {
            _cb = cb.clone();

            _args[0] = &_a1;
            _args[1] = &_a2;
            _args[2] = &_a3;
            _args[3] = &_a4;
            _args[4] = &_a5;
            _args[5] = &_a6;
            _args[6] = &_a7;
            _args[7] = &_a8;
            _args[8] = &_a9;
        }

        ~BasicServiceProcedure()
        {
            delete _cb;
        }

        ServiceProcedure* clone() const
        {
            return new BasicServiceProcedure(*_cb);
        }

        IComposers* beginCall()
        {
            _a1.begin(_v1);
            _a2.begin(_v2);
            _a3.begin(_v3);
            _a4.begin(_v4);
            _a5.begin(_v5);
            _a6.begin(_v6);
            _a7.begin(_v7);
            _a8.begin(_v8);
            _a9.begin(_v9);

            return &_composers;
        }

        IDecomposer* endCall()
        {
            _rv = _cb->call(_v1, _v2, _v3, _v4, _v5, _v6, _v7, _v8, _v9);
            _r.begin(_rv);
            return &_r;
        }

    private:
        typedef typename TypeTraits<A1>::Value V1;
        typedef typename TypeTraits<A2>::Value V2;
        typedef typename TypeTraits<A3>::Value V3;
        typedef typename TypeTraits<A4>::Value V4;
        typedef typename TypeTraits<A5>::Value V5;
        typedef typename TypeTraits<A6>::Value V6;
        typedef typename TypeTraits<A7>::Value V7;
        typedef typename TypeTraits<A8>::Value V8;
        typedef typename TypeTraits<A9>::Value V9;
        typedef typename TypeTraits<R>::Value RV;

        Callable<R, A1, A2, A3, A4, A5, A6, A7, A8, A9>* _cb;
        RV _rv;
        V1 _v1;
        V2 _v2;
        V3 _v3;
        V4 _v4;
        V5 _v5;
        V6 _v6;
        V7 _v7;
        V8 _v8;
        V9 _v9;

        IComposer* _args[9];
        Composers _composers;
        Composer<V1> _a1;
        Composer<V2> _a2;
        Composer<V3> _a3;
        Composer<V4> _a4;
        Composer<V5> _a5;
        Composer<V6> _a6;
        Composer<V7> _a7;
        Composer<V8> _a8;
        Composer<V9> _a9;
        Decomposer<RV> _r;
};


// BasicServiceProcedure with 8 arguments
template <typename R,
          typename A1,
          typename A2,
          typename A3,
          typename A4,
          typename A5,
          typename A6,
          typename A7,
          typename A8>
class BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7, A8,
                            Void, Void> : public ServiceProcedure
{
    public:
        explicit BasicServiceProcedure(const Callable<R, A1, A2, A3, A4, A5, A6, A7, A8>& cb)
        : ServiceProcedure(),
          _cb(0),
          _composers(_args, 8)
        {
            _cb = cb.clone();

            _args[0] = &_a1;
            _args[1] = &_a2;
            _args[2] = &_a3;
            _args[3] = &_a4;
            _args[4] = &_a5;
            _args[5] = &_a6;
            _args[6] = &_a7;
            _args[7] = &_a8;
        }

        ~BasicServiceProcedure()
        {
            delete _cb;
        }

        ServiceProcedure* clone() const
        {
            return new BasicServiceProcedure(*_cb);
        }

        IComposers* beginCall()
        {
            _a1.begin(_v1);
            _a2.begin(_v2);
            _a3.begin(_v3);
            _a4.begin(_v4);
            _a5.begin(_v5);
            _a6.begin(_v6);
            _a7.begin(_v7);
            _a8.begin(_v8);

            return &_composers;
        }

        IDecomposer* endCall()
        {
            _rv = _cb->call(_v1, _v2, _v3, _v4, _v5, _v6, _v7, _v8);
            _r.begin(_rv);
            return &_r;
        }

    private:
        typedef typename TypeTraits<A1>::Value V1;
        typedef typename TypeTraits<A2>::Value V2;
        typedef typename TypeTraits<A3>::Value V3;
        typedef typename TypeTraits<A4>::Value V4;
        typedef typename TypeTraits<A5>::Value V5;
        typedef typename TypeTraits<A6>::Value V6;
        typedef typename TypeTraits<A7>::Value V7;
        typedef typename TypeTraits<A8>::Value V8;
        typedef typename TypeTraits<R>::Value RV;

        Callable<R, A1, A2, A3, A4, A5, A6, A7, A8>* _cb;
        RV _rv;
        V1 _v1;
        V2 _v2;
        V3 _v3;
        V4 _v4;
        V5 _v5;
        V6 _v6;
        V7 _v7;
        V8 _v8;

        IComposer* _args[8];
        Composers _composers;
        Composer<V1> _a1;
        Composer<V2> _a2;
        Composer<V3> _a3;
        Composer<V4> _a4;
        Composer<V5> _a5;
        Composer<V6> _a6;
        Composer<V7> _a7;
        Composer<V8> _a8;
        Decomposer<RV> _r;
};


// BasicServiceProcedure with 7 arguments
template <typename R,
          typename A1,
          typename A2,
          typename A3,
          typename A4,
          typename A5,
          typename A6,
          typename A7>
class BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6, A7,
                            Void, Void, Void> : public ServiceProcedure
{
    public:
        explicit BasicServiceProcedure(const Callable<R, A1, A2, A3, A4, A5, A6, A7>& cb)
        : ServiceProcedure(),
          _cb(0),
          _composers(_args, 7)
        {
            _cb = cb.clone();

            _args[0] = &_a1;
            _args[1] = &_a2;
            _args[2] = &_a3;
            _args[3] = &_a4;
            _args[4] = &_a5;
            _args[5] = &_a6;
            _args[6] = &_a7;
        }

        ~BasicServiceProcedure()
        {
            delete _cb;
        }

        ServiceProcedure* clone() const
        {
            return new BasicServiceProcedure(*_cb);
        }

        IComposers* beginCall()
        {
            _a1.begin(_v1);
            _a2.begin(_v2);
            _a3.begin(_v3);
            _a4.begin(_v4);
            _a5.begin(_v5);
            _a6.begin(_v6);
            _a7.begin(_v7);

            return &_composers;
        }

        IDecomposer* endCall()
        {
            _rv = _cb->call(_v1, _v2, _v3, _v4, _v5, _v6, _v7);
            _r.begin(_rv);
            return &_r;
        }

    private:
        typedef typename TypeTraits<A1>::Value V1;
        typedef typename TypeTraits<A2>::Value V2;
        typedef typename TypeTraits<A3>::Value V3;
        typedef typename TypeTraits<A4>::Value V4;
        typedef typename TypeTraits<A5>::Value V5;
        typedef typename TypeTraits<A6>::Value V6;
        typedef typename TypeTraits<A7>::Value V7;
        typedef typename TypeTraits<R>::Value RV;

        Callable<R, A1, A2, A3, A4, A5, A6, A7>* _cb;
        RV _rv;
        V1 _v1;
        V2 _v2;
        V3 _v3;
        V4 _v4;
        V5 _v5;
        V6 _v6;
        V7 _v7;

        IComposer* _args[7];
        Composers _composers;
        Composer<V1> _a1;
        Composer<V2> _a2;
        Composer<V3> _a3;
        Composer<V4> _a4;
        Composer<V5> _a5;
        Composer<V6> _a6;
        Composer<V7> _a7;
        Decomposer<RV> _r;
};


// BasicServiceProcedure with 6 arguments
template <typename R,
          typename A1,
          typename A2,
          typename A3,
          typename A4,
          typename A5,
          typename A6>
class BasicServiceProcedure<R, A1, A2, A3, A4, A5, A6,
                            Void, Void, Void, Void> : public ServiceProcedure
{
    public:
        explicit BasicServiceProcedure(const Callable<R, A1, A2, A3, A4, A5, A6>& cb)
        : ServiceProcedure(),
          _cb(0),
          _composers(_args, 6)
        {
            _cb = cb.clone();

            _args[0] = &_a1;
            _args[1] = &_a2;
            _args[2] = &_a3;
            _args[3] = &_a4;
            _args[4] = &_a5;
            _args[5] = &_a6;
        }

        ~BasicServiceProcedure()
        {
            delete _cb;
        }

        ServiceProcedure* clone() const
        {
            return new BasicServiceProcedure(*_cb);
        }

        IComposers* beginCall()
        {
            _a1.begin(_v1);
            _a2.begin(_v2);
            _a3.begin(_v3);
            _a4.begin(_v4);
            _a5.begin(_v5);
            _a6.begin(_v6);

            return &_composers;
        }

        IDecomposer* endCall()
        {
            _rv = _cb->call(_v1, _v2, _v3, _v4, _v5, _v6);
            _r.begin(_rv);
            return &_r;
        }

    private:
        typedef typename TypeTraits<A1>::Value V1;
        typedef typename TypeTraits<A2>::Value V2;
        typedef typename TypeTraits<A3>::Value V3;
        typedef typename TypeTraits<A4>::Value V4;
        typedef typename TypeTraits<A5>::Value V5;
        typedef typename TypeTraits<A6>::Value V6;
        typedef typename TypeTraits<R>::Value RV;

        Callable<R, A1, A2, A3, A4, A5, A6>* _cb;
        RV _rv;
        V1 _v1;
        V2 _v2;
        V3 _v3;
        V4 _v4;
        V5 _v5;
        V6 _v6;

        IComposer* _args[6];
        Composers _composers;
        Composer<V1> _a1;
        Composer<V2> _a2;
        Composer<V3> _a3;
        Composer<V4> _a4;
        Composer<V5> _a5;
        Composer<V6> _a6;
        Decomposer<RV> _r;
};


// BasicServiceProcedure with 5 arguments
template <typename R,
          typename A1,
          typename A2,
          typename A3,
          typename A4,
          typename A5>
class BasicServiceProcedure<R, A1, A2, A3, A4, A5,
                            Void, Void, Void, Void, Void> : public ServiceProcedure
{
    public:
        explicit BasicServiceProcedure(const Callable<R, A1, A2, A3, A4, A5>& cb)
        : ServiceProcedure(),
          _cb(0),
          _composers(_args, 5)
        {
            _cb = cb.clone();

            _args[0] = &_a1;
            _args[1] = &_a2;
            _args[2] = &_a3;
            _args[3] = &_a4;
            _args[4] = &_a5;
        }

        ~BasicServiceProcedure()
        {
            delete _cb;
        }

        ServiceProcedure* clone() const
        {
            return new BasicServiceProcedure(*_cb);
        }

        IComposers* beginCall()
        {
            _a1.begin(_v1);
            _a2.begin(_v2);
            _a3.begin(_v3);
            _a4.begin(_v4);
            _a5.begin(_v5);

            return &_composers;
        }

        IDecomposer* endCall()
        {
            _rv = _cb->call(_v1, _v2, _v3, _v4, _v5);
            _r.begin(_rv);
            return &_r;
        }

    private:
        typedef typename TypeTraits<A1>::Value V1;
        typedef typename TypeTraits<A2>::Value V2;
        typedef typename TypeTraits<A3>::Value V3;
        typedef typename TypeTraits<A4>::Value V4;
        typedef typename TypeTraits<A5>::Value V5;
        typedef typename TypeTraits<R>::Value RV;

        Callable<R, A1, A2, A3, A4, A5>* _cb;
        RV _rv;
        V1 _v1;
        V2 _v2;
        V3 _v3;
        V4 _v4;
        V5 _v5;

        IComposer* _args[5];
        Composers _composers;
        Composer<V1> _a1;
        Composer<V2> _a2;
        Composer<V3> _a3;
        Composer<V4> _a4;
        Composer<V5> _a5;
        Decomposer<RV> _r;
};


// BasicServiceProcedure with 4 arguments
template <typename R,
          typename A1,
          typename A2,
          typename A3,
          typename A4>
class BasicServiceProcedure<R, A1, A2, A3, A4,
                            Void, Void, Void, Void, Void, Void> : public ServiceProcedure
{
    public:
        explicit BasicServiceProcedure(const Callable<R, A1, A2, A3, A4>& cb)
        : ServiceProcedure(),
          _cb(0),
          _composers(_args, 4)
        {
            _cb = cb.clone();

            _args[0] = &_a1;
            _args[1] = &_a2;
            _args[2] = &_a3;
            _args[3] = &_a4;
        }

        ~BasicServiceProcedure()
        {
            delete _cb;
        }

        ServiceProcedure* clone() const
        {
            return new BasicServiceProcedure(*_cb);
        }

        IComposers* beginCall()
        {
            _a1.begin(_v1);
            _a2.begin(_v2);
            _a3.begin(_v3);
            _a4.begin(_v4);

            return &_composers;
        }

        IDecomposer* endCall()
        {
            _rv = _cb->call(_v1, _v2, _v3, _v4);
            _r.begin(_rv);
            return &_r;
        }

    private:
        typedef typename TypeTraits<A1>::Value V1;
        typedef typename TypeTraits<A2>::Value V2;
        typedef typename TypeTraits<A3>::Value V3;
        typedef typename TypeTraits<A4>::Value V4;
        typedef typename TypeTraits<R>::Value RV;

        Callable<R, A1, A2, A3, A4>* _cb;
        RV _rv;
        V1 _v1;
        V2 _v2;
        V3 _v3;
        V4 _v4;

        IComposer* _args[4];
        Composers _composers;
        Composer<V1> _a1;
        Composer<V2> _a2;
        Composer<V3> _a3;
        Composer<V4> _a4;
        Decomposer<RV> _r;
};


// BasicServiceProcedure with 3 arguments
template <typename R,
          typename A1,
          typename A2,
          typename A3>
class BasicServiceProcedure<R, A1, A2, A3,
                            Void, Void, Void, Void, Void, Void, Void> : public ServiceProcedure
{
    public:
        explicit BasicServiceProcedure(const Callable<R, A1, A2, A3>& cb)
        : ServiceProcedure(),
          _cb(0),
          _composers(_args, 3)
        {
            _cb = cb.clone();

            _args[0] = &_a1;
            _args[1] = &_a2;
            _args[2] = &_a3;
        }

        ~BasicServiceProcedure()
        {
            delete _cb;
        }

        ServiceProcedure* clone() const
        {
            return new BasicServiceProcedure(*_cb);
        }

        IComposers* beginCall()
        {
            _a1.begin(_v1);
            _a2.begin(_v2);
            _a3.begin(_v3);

            return &_composers;
        }

        IDecomposer* endCall()
        {
            _rv = _cb->call(_v1, _v2, _v3);
            _r.begin(_rv);
            return &_r;
        }

    private:
        typedef typename TypeTraits<A1>::Value V1;
        typedef typename TypeTraits<A2>::Value V2;
        typedef typename TypeTraits<A3>::Value V3;
        typedef typename TypeTraits<R>::Value RV;

        Callable<R, A1, A2, A3>* _cb;
        RV _rv;
        V1 _v1;
        V2 _v2;
        V3 _v3;

        IComposer* _args[3];
        Composers _composers;
        Composer<V1> _a1;
        Composer<V2> _a2;
        Composer<V3> _a3;
        Decomposer<RV> _r;
};


// BasicServiceProcedure with 2 arguments
template <typename R,
          typename A1,
          typename A2>
class BasicServiceProcedure<R, A1, A2,
                            Void, Void, Void, Void, Void, Void, Void, Void> : public ServiceProcedure
{
    public:
        explicit BasicServiceProcedure(const Callable<R, A1, A2>& cb)
        : ServiceProcedure(),
          _cb(0),
          _composers(_args, 2)
        {
            _cb = cb.clone();

            _args[0] = &_a1;
            _args[1] = &_a2;
        }

        ~BasicServiceProcedure()
        {
            delete _cb;
        }

        ServiceProcedure* clone() const
        {
            return new BasicServiceProcedure(*_cb);
        }

        IComposers* beginCall()
        {
            _a1.begin(_v1);
            _a2.begin(_v2);

            return &_composers;
        }

        IDecomposer* endCall()
        {
            _rv = _cb->call(_v1, _v2);
            _r.begin(_rv);
            return &_r;
        }

    private:
        typedef typename TypeTraits<A1>::Value V1;
        typedef typename TypeTraits<A2>::Value V2;
        typedef typename TypeTraits<R>::Value RV;

        Callable<R, A1, A2>* _cb;
        RV _rv;
        V1 _v1;
        V2 _v2;

        IComposer* _args[2];
        Composers _composers;
        Composer<V1> _a1;
        Composer<V2> _a2;
        Decomposer<RV> _r;
};


// BasicServiceProcedure with 1 arguments
template <typename R,
          typename A1>
class BasicServiceProcedure<R, A1,
                            Void, Void, Void, Void, Void, Void, Void, Void, Void> : public ServiceProcedure
{
    public:
        explicit BasicServiceProcedure(const Callable<R, A1>& cb)
        : ServiceProcedure(),
          _cb(0),
          _composers(_args, 1)
        {
            _cb = cb.clone();

            _args[0] = &_a1;
        }

        ~BasicServiceProcedure()
        {
            delete _cb;
        }

        ServiceProcedure* clone() const
        {
            return new BasicServiceProcedure(*_cb);
        }

        IComposers* beginCall()
        {
            _a1.begin(_v1);

            return &_composers;
        }

        IDecomposer* endCall()
        {
            _rv = _cb->call(_v1);
            _r.begin(_rv);
            return &_r;
        }

    private:
        typedef typename TypeTraits<A1>::Value V1;
        typedef typename TypeTraits<R>::Value RV;

        Callable<R, A1>* _cb;
        RV _rv;
        V1 _v1;

        IComposer* _args[1];
        Composers _composers;
        Composer<V1> _a1;
        Decomposer<RV> _r;
};


// BasicServiceProcedure with 0 arguments
template <typename R>
class BasicServiceProcedure<R,
                            Void, Void, Void, Void, Void, Void, Void, Void, Void, Void> : public ServiceProcedure
{
    public:
        explicit BasicServiceProcedure(const Callable<R>& cb)
        : ServiceProcedure(),
          _cb(0),
          _composers(nullptr, 0)
        {
            _cb = cb.clone();
        }

        ~BasicServiceProcedure()
        {
            delete _cb;
        }

        ServiceProcedure* clone() const
        {
            return new BasicServiceProcedure(*_cb);
        }

        IComposers* beginCall()
        {

            return &_composers;
        }

        IDecomposer* endCall()
        {
            _rv = _cb->call();
            _r.begin(_rv);
            return &_r;
        }

    private:
        typedef typename TypeTraits<R>::Value RV;

        Callable<R>* _cb;
        RV _rv;

        Composers _composers;
        Decomposer<RV> _r;
};

//! @endcond internal
// end generated with `serviceprocedure.pl -n 10`
