// generated with `remoteprocedure.pl -n 10`
//! @cond internal
template <typename R,
          typename A1 = cxxtools::Void,
          typename A2 = cxxtools::Void,
          typename A3 = cxxtools::Void,
          typename A4 = cxxtools::Void,
          typename A5 = cxxtools::Void,
          typename A6 = cxxtools::Void,
          typename A7 = cxxtools::Void,
          typename A8 = cxxtools::Void,
          typename A9 = cxxtools::Void,
          typename A10 = cxxtools::Void>
class RemoteProcedure : public RemoteProcedureBase<R>
{
    public:
        RemoteProcedure(RemoteClient& client, const String& name)
        : RemoteProcedureBase<R>(client, name)
        { }

        RemoteProcedure(RemoteClient& client, const char* name)
        : RemoteProcedureBase<R>(client, String(name))
        { }

        void begin(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9, const A10& a10)
        {
            this->_result.clearFault();

            _a1.begin(a1);
            _a2.begin(a2);
            _a3.begin(a3);
            _a4.begin(a4);
            _a5.begin(a5);
            _a6.begin(a6);
            _a7.begin(a7);
            _a8.begin(a8);
            _a9.begin(a9);
            _a10.begin(a10);

            this->_r.begin(this->_result.value());

            IDecomposer* argv[10] = { &_a1, &_a2, &_a3, &_a4, &_a5, &_a6, &_a7, &_a8, &_a9, &_a10 };
            this->client().beginCall(this->_r, *this, argv, 10);
        }

        R&& call(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9, const A10& a10)
        {
            this->_result.clearFault();

            _a1.begin(a1);
            _a2.begin(a2);
            _a3.begin(a3);
            _a4.begin(a4);
            _a5.begin(a5);
            _a6.begin(a6);
            _a7.begin(a7);
            _a8.begin(a8);
            _a9.begin(a9);
            _a10.begin(a10);
            this->_r.begin(this->_result.value());

            IDecomposer* argv[10] = { &_a1, &_a2, &_a3, &_a4, &_a5, &_a6, &_a7, &_a8, &_a9, &_a10 };
            this->client().call(this->_r, *this, argv, 10);
            return this->_result.get();
        }

        R&& operator()(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9, const A10& a10)
        {
            return this->call(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);
        }

    private:
        Decomposer<A1> _a1;
        Decomposer<A2> _a2;
        Decomposer<A3> _a3;
        Decomposer<A4> _a4;
        Decomposer<A5> _a5;
        Decomposer<A6> _a6;
        Decomposer<A7> _a7;
        Decomposer<A8> _a8;
        Decomposer<A9> _a9;
        Decomposer<A10> _a10;
};


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
class RemoteProcedure<R, A1, A2, A3, A4, A5, A6, A7, A8, A9,
                      cxxtools::Void> : public RemoteProcedureBase<R>
{
    public:
        RemoteProcedure(RemoteClient& client, const String& name)
        : RemoteProcedureBase<R>(client, name)
        { }

        RemoteProcedure(RemoteClient& client, const char* name)
        : RemoteProcedureBase<R>(client, String(name))
        { }

        void begin(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9)
        {
            this->_result.clearFault();

            _a1.begin(a1);
            _a2.begin(a2);
            _a3.begin(a3);
            _a4.begin(a4);
            _a5.begin(a5);
            _a6.begin(a6);
            _a7.begin(a7);
            _a8.begin(a8);
            _a9.begin(a9);

            this->_r.begin(this->_result.value());

            IDecomposer* argv[9] = { &_a1, &_a2, &_a3, &_a4, &_a5, &_a6, &_a7, &_a8, &_a9 };
            this->client().beginCall(this->_r, *this, argv, 9);
        }

        R&& call(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9)
        {
            this->_result.clearFault();

            _a1.begin(a1);
            _a2.begin(a2);
            _a3.begin(a3);
            _a4.begin(a4);
            _a5.begin(a5);
            _a6.begin(a6);
            _a7.begin(a7);
            _a8.begin(a8);
            _a9.begin(a9);
            this->_r.begin(this->_result.value());

            IDecomposer* argv[9] = { &_a1, &_a2, &_a3, &_a4, &_a5, &_a6, &_a7, &_a8, &_a9 };
            this->client().call(this->_r, *this, argv, 9);
            return this->_result.get();
        }

        R&& operator()(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8, const A9& a9)
        {
            return this->call(a1, a2, a3, a4, a5, a6, a7, a8, a9);
        }

    private:
        Decomposer<A1> _a1;
        Decomposer<A2> _a2;
        Decomposer<A3> _a3;
        Decomposer<A4> _a4;
        Decomposer<A5> _a5;
        Decomposer<A6> _a6;
        Decomposer<A7> _a7;
        Decomposer<A8> _a8;
        Decomposer<A9> _a9;
};


template <typename R,
          typename A1,
          typename A2,
          typename A3,
          typename A4,
          typename A5,
          typename A6,
          typename A7,
          typename A8>
class RemoteProcedure<R, A1, A2, A3, A4, A5, A6, A7, A8,
                      cxxtools::Void,
                      cxxtools::Void> : public RemoteProcedureBase<R>
{
    public:
        RemoteProcedure(RemoteClient& client, const String& name)
        : RemoteProcedureBase<R>(client, name)
        { }

        RemoteProcedure(RemoteClient& client, const char* name)
        : RemoteProcedureBase<R>(client, String(name))
        { }

        void begin(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8)
        {
            this->_result.clearFault();

            _a1.begin(a1);
            _a2.begin(a2);
            _a3.begin(a3);
            _a4.begin(a4);
            _a5.begin(a5);
            _a6.begin(a6);
            _a7.begin(a7);
            _a8.begin(a8);

            this->_r.begin(this->_result.value());

            IDecomposer* argv[8] = { &_a1, &_a2, &_a3, &_a4, &_a5, &_a6, &_a7, &_a8 };
            this->client().beginCall(this->_r, *this, argv, 8);
        }

        R&& call(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8)
        {
            this->_result.clearFault();

            _a1.begin(a1);
            _a2.begin(a2);
            _a3.begin(a3);
            _a4.begin(a4);
            _a5.begin(a5);
            _a6.begin(a6);
            _a7.begin(a7);
            _a8.begin(a8);
            this->_r.begin(this->_result.value());

            IDecomposer* argv[8] = { &_a1, &_a2, &_a3, &_a4, &_a5, &_a6, &_a7, &_a8 };
            this->client().call(this->_r, *this, argv, 8);
            return this->_result.get();
        }

        R&& operator()(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7, const A8& a8)
        {
            return this->call(a1, a2, a3, a4, a5, a6, a7, a8);
        }

    private:
        Decomposer<A1> _a1;
        Decomposer<A2> _a2;
        Decomposer<A3> _a3;
        Decomposer<A4> _a4;
        Decomposer<A5> _a5;
        Decomposer<A6> _a6;
        Decomposer<A7> _a7;
        Decomposer<A8> _a8;
};


template <typename R,
          typename A1,
          typename A2,
          typename A3,
          typename A4,
          typename A5,
          typename A6,
          typename A7>
class RemoteProcedure<R, A1, A2, A3, A4, A5, A6, A7,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void> : public RemoteProcedureBase<R>
{
    public:
        RemoteProcedure(RemoteClient& client, const String& name)
        : RemoteProcedureBase<R>(client, name)
        { }

        RemoteProcedure(RemoteClient& client, const char* name)
        : RemoteProcedureBase<R>(client, String(name))
        { }

        void begin(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7)
        {
            this->_result.clearFault();

            _a1.begin(a1);
            _a2.begin(a2);
            _a3.begin(a3);
            _a4.begin(a4);
            _a5.begin(a5);
            _a6.begin(a6);
            _a7.begin(a7);

            this->_r.begin(this->_result.value());

            IDecomposer* argv[7] = { &_a1, &_a2, &_a3, &_a4, &_a5, &_a6, &_a7 };
            this->client().beginCall(this->_r, *this, argv, 7);
        }

        R&& call(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7)
        {
            this->_result.clearFault();

            _a1.begin(a1);
            _a2.begin(a2);
            _a3.begin(a3);
            _a4.begin(a4);
            _a5.begin(a5);
            _a6.begin(a6);
            _a7.begin(a7);
            this->_r.begin(this->_result.value());

            IDecomposer* argv[7] = { &_a1, &_a2, &_a3, &_a4, &_a5, &_a6, &_a7 };
            this->client().call(this->_r, *this, argv, 7);
            return this->_result.get();
        }

        R&& operator()(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6, const A7& a7)
        {
            return this->call(a1, a2, a3, a4, a5, a6, a7);
        }

    private:
        Decomposer<A1> _a1;
        Decomposer<A2> _a2;
        Decomposer<A3> _a3;
        Decomposer<A4> _a4;
        Decomposer<A5> _a5;
        Decomposer<A6> _a6;
        Decomposer<A7> _a7;
};


template <typename R,
          typename A1,
          typename A2,
          typename A3,
          typename A4,
          typename A5,
          typename A6>
class RemoteProcedure<R, A1, A2, A3, A4, A5, A6,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void> : public RemoteProcedureBase<R>
{
    public:
        RemoteProcedure(RemoteClient& client, const String& name)
        : RemoteProcedureBase<R>(client, name)
        { }

        RemoteProcedure(RemoteClient& client, const char* name)
        : RemoteProcedureBase<R>(client, String(name))
        { }

        void begin(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6)
        {
            this->_result.clearFault();

            _a1.begin(a1);
            _a2.begin(a2);
            _a3.begin(a3);
            _a4.begin(a4);
            _a5.begin(a5);
            _a6.begin(a6);

            this->_r.begin(this->_result.value());

            IDecomposer* argv[6] = { &_a1, &_a2, &_a3, &_a4, &_a5, &_a6 };
            this->client().beginCall(this->_r, *this, argv, 6);
        }

        R&& call(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6)
        {
            this->_result.clearFault();

            _a1.begin(a1);
            _a2.begin(a2);
            _a3.begin(a3);
            _a4.begin(a4);
            _a5.begin(a5);
            _a6.begin(a6);
            this->_r.begin(this->_result.value());

            IDecomposer* argv[6] = { &_a1, &_a2, &_a3, &_a4, &_a5, &_a6 };
            this->client().call(this->_r, *this, argv, 6);
            return this->_result.get();
        }

        R&& operator()(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5, const A6& a6)
        {
            return this->call(a1, a2, a3, a4, a5, a6);
        }

    private:
        Decomposer<A1> _a1;
        Decomposer<A2> _a2;
        Decomposer<A3> _a3;
        Decomposer<A4> _a4;
        Decomposer<A5> _a5;
        Decomposer<A6> _a6;
};


template <typename R,
          typename A1,
          typename A2,
          typename A3,
          typename A4,
          typename A5>
class RemoteProcedure<R, A1, A2, A3, A4, A5,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void> : public RemoteProcedureBase<R>
{
    public:
        RemoteProcedure(RemoteClient& client, const String& name)
        : RemoteProcedureBase<R>(client, name)
        { }

        RemoteProcedure(RemoteClient& client, const char* name)
        : RemoteProcedureBase<R>(client, String(name))
        { }

        void begin(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5)
        {
            this->_result.clearFault();

            _a1.begin(a1);
            _a2.begin(a2);
            _a3.begin(a3);
            _a4.begin(a4);
            _a5.begin(a5);

            this->_r.begin(this->_result.value());

            IDecomposer* argv[5] = { &_a1, &_a2, &_a3, &_a4, &_a5 };
            this->client().beginCall(this->_r, *this, argv, 5);
        }

        R&& call(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5)
        {
            this->_result.clearFault();

            _a1.begin(a1);
            _a2.begin(a2);
            _a3.begin(a3);
            _a4.begin(a4);
            _a5.begin(a5);
            this->_r.begin(this->_result.value());

            IDecomposer* argv[5] = { &_a1, &_a2, &_a3, &_a4, &_a5 };
            this->client().call(this->_r, *this, argv, 5);
            return this->_result.get();
        }

        R&& operator()(const A1& a1, const A2& a2, const A3& a3, const A4& a4, const A5& a5)
        {
            return this->call(a1, a2, a3, a4, a5);
        }

    private:
        Decomposer<A1> _a1;
        Decomposer<A2> _a2;
        Decomposer<A3> _a3;
        Decomposer<A4> _a4;
        Decomposer<A5> _a5;
};


template <typename R,
          typename A1,
          typename A2,
          typename A3,
          typename A4>
class RemoteProcedure<R, A1, A2, A3, A4,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void> : public RemoteProcedureBase<R>
{
    public:
        RemoteProcedure(RemoteClient& client, const String& name)
        : RemoteProcedureBase<R>(client, name)
        { }

        RemoteProcedure(RemoteClient& client, const char* name)
        : RemoteProcedureBase<R>(client, String(name))
        { }

        void begin(const A1& a1, const A2& a2, const A3& a3, const A4& a4)
        {
            this->_result.clearFault();

            _a1.begin(a1);
            _a2.begin(a2);
            _a3.begin(a3);
            _a4.begin(a4);

            this->_r.begin(this->_result.value());

            IDecomposer* argv[4] = { &_a1, &_a2, &_a3, &_a4 };
            this->client().beginCall(this->_r, *this, argv, 4);
        }

        R&& call(const A1& a1, const A2& a2, const A3& a3, const A4& a4)
        {
            this->_result.clearFault();

            _a1.begin(a1);
            _a2.begin(a2);
            _a3.begin(a3);
            _a4.begin(a4);
            this->_r.begin(this->_result.value());

            IDecomposer* argv[4] = { &_a1, &_a2, &_a3, &_a4 };
            this->client().call(this->_r, *this, argv, 4);
            return this->_result.get();
        }

        R&& operator()(const A1& a1, const A2& a2, const A3& a3, const A4& a4)
        {
            return this->call(a1, a2, a3, a4);
        }

    private:
        Decomposer<A1> _a1;
        Decomposer<A2> _a2;
        Decomposer<A3> _a3;
        Decomposer<A4> _a4;
};


template <typename R,
          typename A1,
          typename A2,
          typename A3>
class RemoteProcedure<R, A1, A2, A3,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void> : public RemoteProcedureBase<R>
{
    public:
        RemoteProcedure(RemoteClient& client, const String& name)
        : RemoteProcedureBase<R>(client, name)
        { }

        RemoteProcedure(RemoteClient& client, const char* name)
        : RemoteProcedureBase<R>(client, String(name))
        { }

        void begin(const A1& a1, const A2& a2, const A3& a3)
        {
            this->_result.clearFault();

            _a1.begin(a1);
            _a2.begin(a2);
            _a3.begin(a3);

            this->_r.begin(this->_result.value());

            IDecomposer* argv[3] = { &_a1, &_a2, &_a3 };
            this->client().beginCall(this->_r, *this, argv, 3);
        }

        R&& call(const A1& a1, const A2& a2, const A3& a3)
        {
            this->_result.clearFault();

            _a1.begin(a1);
            _a2.begin(a2);
            _a3.begin(a3);
            this->_r.begin(this->_result.value());

            IDecomposer* argv[3] = { &_a1, &_a2, &_a3 };
            this->client().call(this->_r, *this, argv, 3);
            return this->_result.get();
        }

        R&& operator()(const A1& a1, const A2& a2, const A3& a3)
        {
            return this->call(a1, a2, a3);
        }

    private:
        Decomposer<A1> _a1;
        Decomposer<A2> _a2;
        Decomposer<A3> _a3;
};


template <typename R,
          typename A1,
          typename A2>
class RemoteProcedure<R, A1, A2,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void> : public RemoteProcedureBase<R>
{
    public:
        RemoteProcedure(RemoteClient& client, const String& name)
        : RemoteProcedureBase<R>(client, name)
        { }

        RemoteProcedure(RemoteClient& client, const char* name)
        : RemoteProcedureBase<R>(client, String(name))
        { }

        void begin(const A1& a1, const A2& a2)
        {
            this->_result.clearFault();

            _a1.begin(a1);
            _a2.begin(a2);

            this->_r.begin(this->_result.value());

            IDecomposer* argv[2] = { &_a1, &_a2 };
            this->client().beginCall(this->_r, *this, argv, 2);
        }

        R&& call(const A1& a1, const A2& a2)
        {
            this->_result.clearFault();

            _a1.begin(a1);
            _a2.begin(a2);
            this->_r.begin(this->_result.value());

            IDecomposer* argv[2] = { &_a1, &_a2 };
            this->client().call(this->_r, *this, argv, 2);
            return this->_result.get();
        }

        R&& operator()(const A1& a1, const A2& a2)
        {
            return this->call(a1, a2);
        }

    private:
        Decomposer<A1> _a1;
        Decomposer<A2> _a2;
};


template <typename R,
          typename A1>
class RemoteProcedure<R, A1,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void> : public RemoteProcedureBase<R>
{
    public:
        RemoteProcedure(RemoteClient& client, const String& name)
        : RemoteProcedureBase<R>(client, name)
        { }

        RemoteProcedure(RemoteClient& client, const char* name)
        : RemoteProcedureBase<R>(client, String(name))
        { }

        void begin(const A1& a1)
        {
            this->_result.clearFault();

            _a1.begin(a1);

            this->_r.begin(this->_result.value());

            IDecomposer* argv[1] = { &_a1 };
            this->client().beginCall(this->_r, *this, argv, 1);
        }

        R&& call(const A1& a1)
        {
            this->_result.clearFault();

            _a1.begin(a1);
            this->_r.begin(this->_result.value());

            IDecomposer* argv[1] = { &_a1 };
            this->client().call(this->_r, *this, argv, 1);
            return this->_result.get();
        }

        R&& operator()(const A1& a1)
        {
            return this->call(a1);
        }

    private:
        Decomposer<A1> _a1;
};


template <typename R>
class RemoteProcedure<R,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void,
                      cxxtools::Void> : public RemoteProcedureBase<R>
{
    public:
        RemoteProcedure(RemoteClient& client, const String& name)
        : RemoteProcedureBase<R>(client, name)
        { }

        RemoteProcedure(RemoteClient& client, const char* name)
        : RemoteProcedureBase<R>(client, String(name))
        { }

        void begin()
        {
            this->_result.clearFault();

            this->_r.begin(this->_result.value());

            IDecomposer* argv[1] = { 0 };
            this->client().beginCall(this->_r, *this, argv, 0);
        }

        R&& call()
        {
            this->_result.clearFault();

            this->_r.begin(this->_result.value());

            IDecomposer* argv[1] = { 0 };
            this->client().call(this->_r, *this, argv, 0);
            return this->_result.get();
        }

        R&& operator()()
        {
            return this->call();
        }
};

//! @endcond internal
// end generated with `remoteprocedure.pl -n 10`
