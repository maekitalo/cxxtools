/**
 The Function class wraps std functions in the form of a Callable, for use
with the signals/slots framework.
*/

template <typename R, class A1 = Void, class A2 = Void, class A3 = Void, class A4 = Void, class A5 = Void, class A6 = Void, class A7 = Void, class A8 = Void, class A9 = Void, class A10 = Void>
class Function;

template<typename T, typename... U>
size_t getAddress(std::function<T(U...)> f) {
    typedef T(fnType)(U...);
    fnType ** fnPointer = f.template target<fnType*>();
    return (size_t) *fnPointer;
}

/** FunctionSlot wraps Function objects so that they can act as Slots.
*/
template <typename R, class A1 = Void, class A2 = Void, class A3 = Void, class A4 = Void, class A5 = Void, class A6 = Void, class A7 = Void, class A8 = Void, class A9 = Void, class A10 = Void>
class FunctionSlot : public BasicSlot<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>
{
    public:
        FunctionSlot(const Function<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>& func)
        : _func( func )
        {}

        /** Returns a pointer to this object's internal Callable. */
        virtual const void* callable() const
        { return &_func; }

        /** Creates a copy of this object and returns it. Caller owns the returned object. */
        Slot* clone() const
        { return new FunctionSlot(*this); }

        virtual void onConnect(const Connection& /*c*/)
        { }

        virtual void onDisconnect(const Connection& /*c*/)
        { }
        virtual bool equals(const Slot& slot) const
        {
            const FunctionSlot* fs = dynamic_cast<const FunctionSlot*>(&slot);
            return fs ? (_func == fs->_func) : false;
        }

    private:
        Function<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10> _func;
}; // FunctionSlot


// BEGIN_Function 10
template <typename R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
class Function : public Callable<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>
{
    public:
        /** The function signature wrapped by this class. */
        typedef std::function<R(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10)> FuncT;

        /** Wraps func. */
        Function(FuncT func)
        : _function(func) { }

        /** Deeply copies f. */
        Function(const Function& f) = default;

        /** Call the wrapped function, passing it the arguments as-is and returning
        its return value. */
        R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10) const
        { return _function(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10); }

        /** Creates a clone of this object and returns it. The caller owns the returned object. */
        Function<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>* clone() const
        { return new Function(*this); }

        /** Returns true if rhs and this object point to the same function. */
        bool operator==(const Function& rhs) const
        {
            return getAddress(_function) == getAddress(rhs._function);
        }

    private:
        FuncT _function;
};

/** Creates and returns a Function wrapper for the given free/static function. */
template <typename R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
Function<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10> callable(R (*func)(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10))
{ return Function<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>(func); }

/** Creates and returns a FunctionSlot object for the given free/static function. */
template <typename R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
FunctionSlot<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10> slot( R (*func)(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10) )
{ return FunctionSlot<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>( callable(func) ); }

/** Creates and returns a FunctionSlot object for the given std::function. */
template <typename R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
FunctionSlot<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10> slot( const std::function<R(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10)>& func )
{ return FunctionSlot<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>( func ); }

// END_Function 10
// BEGIN_Function 9
template <typename R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
class Function<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, Void> : public Callable<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, Void>
{
    public:
        /** The function signature wrapped by this class. */
        typedef std::function<R(A1, A2, A3, A4, A5, A6, A7, A8, A9)> FuncT;

        /** Wraps func. */
        Function(FuncT func)
        : _function(func) { }

        /** Deeply copies f. */
        Function(const Function& f) = default;

        /** Call the wrapped function, passing it the arguments as-is and returning
        its return value. */
        R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9) const
        { return _function(a1, a2, a3, a4, a5, a6, a7, a8, a9); }

        /** Creates a clone of this object and returns it. The caller owns the returned object. */
        Function<R, A1, A2, A3, A4, A5, A6, A7, A8, A9>* clone() const
        { return new Function(*this); }

        /** Returns true if rhs and this object point to the same function. */
        bool operator==(const Function& rhs) const
        {
            return getAddress(_function) == getAddress(rhs._function);
        }

    private:
        FuncT _function;
};

/** Creates and returns a Function wrapper for the given free/static function. */
template <typename R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
Function<R, A1, A2, A3, A4, A5, A6, A7, A8, A9> callable(R (*func)(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9))
{ return Function<R, A1, A2, A3, A4, A5, A6, A7, A8, A9>(func); }

/** Creates and returns a FunctionSlot object for the given free/static function. */
template <typename R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
FunctionSlot<R, A1, A2, A3, A4, A5, A6, A7, A8, A9> slot( R (*func)(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9) )
{ return FunctionSlot<R, A1, A2, A3, A4, A5, A6, A7, A8, A9>( callable(func) ); }

/** Creates and returns a FunctionSlot object for the given std::function. */
template <typename R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
FunctionSlot<R, A1, A2, A3, A4, A5, A6, A7, A8, A9> slot( const std::function<R(A1, A2, A3, A4, A5, A6, A7, A8, A9)>& func )
{ return FunctionSlot<R, A1, A2, A3, A4, A5, A6, A7, A8, A9>( func ); }

// END_Function 9
// BEGIN_Function 8
template <typename R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
class Function<R, A1, A2, A3, A4, A5, A6, A7, A8, Void, Void> : public Callable<R, A1, A2, A3, A4, A5, A6, A7, A8, Void, Void>
{
    public:
        /** The function signature wrapped by this class. */
        typedef std::function<R(A1, A2, A3, A4, A5, A6, A7, A8)> FuncT;

        /** Wraps func. */
        Function(FuncT func)
        : _function(func) { }

        /** Deeply copies f. */
        Function(const Function& f) = default;

        /** Call the wrapped function, passing it the arguments as-is and returning
        its return value. */
        R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8) const
        { return _function(a1, a2, a3, a4, a5, a6, a7, a8); }

        /** Creates a clone of this object and returns it. The caller owns the returned object. */
        Function<R, A1, A2, A3, A4, A5, A6, A7, A8>* clone() const
        { return new Function(*this); }

        /** Returns true if rhs and this object point to the same function. */
        bool operator==(const Function& rhs) const
        {
            return getAddress(_function) == getAddress(rhs._function);
        }

    private:
        FuncT _function;
};

/** Creates and returns a Function wrapper for the given free/static function. */
template <typename R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
Function<R, A1, A2, A3, A4, A5, A6, A7, A8> callable(R (*func)(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8))
{ return Function<R, A1, A2, A3, A4, A5, A6, A7, A8>(func); }

/** Creates and returns a FunctionSlot object for the given free/static function. */
template <typename R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
FunctionSlot<R, A1, A2, A3, A4, A5, A6, A7, A8> slot( R (*func)(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8) )
{ return FunctionSlot<R, A1, A2, A3, A4, A5, A6, A7, A8>( callable(func) ); }

/** Creates and returns a FunctionSlot object for the given std::function. */
template <typename R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
FunctionSlot<R, A1, A2, A3, A4, A5, A6, A7, A8> slot( const std::function<R(A1, A2, A3, A4, A5, A6, A7, A8)>& func )
{ return FunctionSlot<R, A1, A2, A3, A4, A5, A6, A7, A8>( func ); }

// END_Function 8
// BEGIN_Function 7
template <typename R, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
class Function<R, A1, A2, A3, A4, A5, A6, A7, Void, Void, Void> : public Callable<R, A1, A2, A3, A4, A5, A6, A7, Void, Void, Void>
{
    public:
        /** The function signature wrapped by this class. */
        typedef std::function<R(A1, A2, A3, A4, A5, A6, A7)> FuncT;

        /** Wraps func. */
        Function(FuncT func)
        : _function(func) { }

        /** Deeply copies f. */
        Function(const Function& f) = default;

        /** Call the wrapped function, passing it the arguments as-is and returning
        its return value. */
        R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7) const
        { return _function(a1, a2, a3, a4, a5, a6, a7); }

        /** Creates a clone of this object and returns it. The caller owns the returned object. */
        Function<R, A1, A2, A3, A4, A5, A6, A7>* clone() const
        { return new Function(*this); }

        /** Returns true if rhs and this object point to the same function. */
        bool operator==(const Function& rhs) const
        {
            return getAddress(_function) == getAddress(rhs._function);
        }

    private:
        FuncT _function;
};

/** Creates and returns a Function wrapper for the given free/static function. */
template <typename R, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
Function<R, A1, A2, A3, A4, A5, A6, A7> callable(R (*func)(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7))
{ return Function<R, A1, A2, A3, A4, A5, A6, A7>(func); }

/** Creates and returns a FunctionSlot object for the given free/static function. */
template <typename R, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
FunctionSlot<R, A1, A2, A3, A4, A5, A6, A7> slot( R (*func)(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7) )
{ return FunctionSlot<R, A1, A2, A3, A4, A5, A6, A7>( callable(func) ); }

/** Creates and returns a FunctionSlot object for the given std::function. */
template <typename R, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
FunctionSlot<R, A1, A2, A3, A4, A5, A6, A7> slot( const std::function<R(A1, A2, A3, A4, A5, A6, A7)>& func )
{ return FunctionSlot<R, A1, A2, A3, A4, A5, A6, A7>( func ); }

// END_Function 7
// BEGIN_Function 6
template <typename R, class A1, class A2, class A3, class A4, class A5, class A6>
class Function<R, A1, A2, A3, A4, A5, A6, Void, Void, Void, Void> : public Callable<R, A1, A2, A3, A4, A5, A6, Void, Void, Void, Void>
{
    public:
        /** The function signature wrapped by this class. */
        typedef std::function<R(A1, A2, A3, A4, A5, A6)> FuncT;

        /** Wraps func. */
        Function(FuncT func)
        : _function(func) { }

        /** Deeply copies f. */
        Function(const Function& f) = default;

        /** Call the wrapped function, passing it the arguments as-is and returning
        its return value. */
        R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) const
        { return _function(a1, a2, a3, a4, a5, a6); }

        /** Creates a clone of this object and returns it. The caller owns the returned object. */
        Function<R, A1, A2, A3, A4, A5, A6>* clone() const
        { return new Function(*this); }

        /** Returns true if rhs and this object point to the same function. */
        bool operator==(const Function& rhs) const
        {
            return getAddress(_function) == getAddress(rhs._function);
        }

    private:
        FuncT _function;
};

/** Creates and returns a Function wrapper for the given free/static function. */
template <typename R, class A1, class A2, class A3, class A4, class A5, class A6>
Function<R, A1, A2, A3, A4, A5, A6> callable(R (*func)(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6))
{ return Function<R, A1, A2, A3, A4, A5, A6>(func); }

/** Creates and returns a FunctionSlot object for the given free/static function. */
template <typename R, class A1, class A2, class A3, class A4, class A5, class A6>
FunctionSlot<R, A1, A2, A3, A4, A5, A6> slot( R (*func)(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) )
{ return FunctionSlot<R, A1, A2, A3, A4, A5, A6>( callable(func) ); }

/** Creates and returns a FunctionSlot object for the given std::function. */
template <typename R, class A1, class A2, class A3, class A4, class A5, class A6>
FunctionSlot<R, A1, A2, A3, A4, A5, A6> slot( const std::function<R(A1, A2, A3, A4, A5, A6)>& func )
{ return FunctionSlot<R, A1, A2, A3, A4, A5, A6>( func ); }

// END_Function 6
// BEGIN_Function 5
template <typename R, class A1, class A2, class A3, class A4, class A5>
class Function<R, A1, A2, A3, A4, A5, Void, Void, Void, Void, Void> : public Callable<R, A1, A2, A3, A4, A5, Void, Void, Void, Void, Void>
{
    public:
        /** The function signature wrapped by this class. */
        typedef std::function<R(A1, A2, A3, A4, A5)> FuncT;

        /** Wraps func. */
        Function(FuncT func)
        : _function(func) { }

        /** Deeply copies f. */
        Function(const Function& f) = default;

        /** Call the wrapped function, passing it the arguments as-is and returning
        its return value. */
        R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) const
        { return _function(a1, a2, a3, a4, a5); }

        /** Creates a clone of this object and returns it. The caller owns the returned object. */
        Function<R, A1, A2, A3, A4, A5>* clone() const
        { return new Function(*this); }

        /** Returns true if rhs and this object point to the same function. */
        bool operator==(const Function& rhs) const
        {
            return getAddress(_function) == getAddress(rhs._function);
        }

    private:
        FuncT _function;
};

/** Creates and returns a Function wrapper for the given free/static function. */
template <typename R, class A1, class A2, class A3, class A4, class A5>
Function<R, A1, A2, A3, A4, A5> callable(R (*func)(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5))
{ return Function<R, A1, A2, A3, A4, A5>(func); }

/** Creates and returns a FunctionSlot object for the given free/static function. */
template <typename R, class A1, class A2, class A3, class A4, class A5>
FunctionSlot<R, A1, A2, A3, A4, A5> slot( R (*func)(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) )
{ return FunctionSlot<R, A1, A2, A3, A4, A5>( callable(func) ); }

/** Creates and returns a FunctionSlot object for the given std::function. */
template <typename R, class A1, class A2, class A3, class A4, class A5>
FunctionSlot<R, A1, A2, A3, A4, A5> slot( const std::function<R(A1, A2, A3, A4, A5)>& func )
{ return FunctionSlot<R, A1, A2, A3, A4, A5>( func ); }

// END_Function 5
// BEGIN_Function 4
template <typename R, class A1, class A2, class A3, class A4>
class Function<R, A1, A2, A3, A4, Void, Void, Void, Void, Void, Void> : public Callable<R, A1, A2, A3, A4, Void, Void, Void, Void, Void, Void>
{
    public:
        /** The function signature wrapped by this class. */
        typedef std::function<R(A1, A2, A3, A4)> FuncT;

        /** Wraps func. */
        Function(FuncT func)
        : _function(func) { }

        /** Deeply copies f. */
        Function(const Function& f) = default;

        /** Call the wrapped function, passing it the arguments as-is and returning
        its return value. */
        R operator()(A1 a1, A2 a2, A3 a3, A4 a4) const
        { return _function(a1, a2, a3, a4); }

        /** Creates a clone of this object and returns it. The caller owns the returned object. */
        Function<R, A1, A2, A3, A4>* clone() const
        { return new Function(*this); }

        /** Returns true if rhs and this object point to the same function. */
        bool operator==(const Function& rhs) const
        {
            return getAddress(_function) == getAddress(rhs._function);
        }

    private:
        FuncT _function;
};

/** Creates and returns a Function wrapper for the given free/static function. */
template <typename R, class A1, class A2, class A3, class A4>
Function<R, A1, A2, A3, A4> callable(R (*func)(A1 a1, A2 a2, A3 a3, A4 a4))
{ return Function<R, A1, A2, A3, A4>(func); }

/** Creates and returns a FunctionSlot object for the given free/static function. */
template <typename R, class A1, class A2, class A3, class A4>
FunctionSlot<R, A1, A2, A3, A4> slot( R (*func)(A1 a1, A2 a2, A3 a3, A4 a4) )
{ return FunctionSlot<R, A1, A2, A3, A4>( callable(func) ); }

/** Creates and returns a FunctionSlot object for the given std::function. */
template <typename R, class A1, class A2, class A3, class A4>
FunctionSlot<R, A1, A2, A3, A4> slot( const std::function<R(A1, A2, A3, A4)>& func )
{ return FunctionSlot<R, A1, A2, A3, A4>( func ); }

// END_Function 4
// BEGIN_Function 3
template <typename R, class A1, class A2, class A3>
class Function<R, A1, A2, A3, Void, Void, Void, Void, Void, Void, Void> : public Callable<R, A1, A2, A3, Void, Void, Void, Void, Void, Void, Void>
{
    public:
        /** The function signature wrapped by this class. */
        typedef std::function<R(A1, A2, A3)> FuncT;

        /** Wraps func. */
        Function(FuncT func)
        : _function(func) { }

        /** Deeply copies f. */
        Function(const Function& f) = default;

        /** Call the wrapped function, passing it the arguments as-is and returning
        its return value. */
        R operator()(A1 a1, A2 a2, A3 a3) const
        { return _function(a1, a2, a3); }

        /** Creates a clone of this object and returns it. The caller owns the returned object. */
        Function<R, A1, A2, A3>* clone() const
        { return new Function(*this); }

        /** Returns true if rhs and this object point to the same function. */
        bool operator==(const Function& rhs) const
        {
            return getAddress(_function) == getAddress(rhs._function);
        }

    private:
        FuncT _function;
};

/** Creates and returns a Function wrapper for the given free/static function. */
template <typename R, class A1, class A2, class A3>
Function<R, A1, A2, A3> callable(R (*func)(A1 a1, A2 a2, A3 a3))
{ return Function<R, A1, A2, A3>(func); }

/** Creates and returns a FunctionSlot object for the given free/static function. */
template <typename R, class A1, class A2, class A3>
FunctionSlot<R, A1, A2, A3> slot( R (*func)(A1 a1, A2 a2, A3 a3) )
{ return FunctionSlot<R, A1, A2, A3>( callable(func) ); }

/** Creates and returns a FunctionSlot object for the given std::function. */
template <typename R, class A1, class A2, class A3>
FunctionSlot<R, A1, A2, A3> slot( const std::function<R(A1, A2, A3)>& func )
{ return FunctionSlot<R, A1, A2, A3>( func ); }

// END_Function 3
// BEGIN_Function 2
template <typename R, class A1, class A2>
class Function<R, A1, A2, Void, Void, Void, Void, Void, Void, Void, Void> : public Callable<R, A1, A2, Void, Void, Void, Void, Void, Void, Void, Void>
{
    public:
        /** The function signature wrapped by this class. */
        typedef std::function<R(A1, A2)> FuncT;

        /** Wraps func. */
        Function(FuncT func)
        : _function(func) { }

        /** Deeply copies f. */
        Function(const Function& f) = default;

        /** Call the wrapped function, passing it the arguments as-is and returning
        its return value. */
        R operator()(A1 a1, A2 a2) const
        { return _function(a1, a2); }

        /** Creates a clone of this object and returns it. The caller owns the returned object. */
        Function<R, A1, A2>* clone() const
        { return new Function(*this); }

        /** Returns true if rhs and this object point to the same function. */
        bool operator==(const Function& rhs) const
        {
            return getAddress(_function) == getAddress(rhs._function);
        }

    private:
        FuncT _function;
};

/** Creates and returns a Function wrapper for the given free/static function. */
template <typename R, class A1, class A2>
Function<R, A1, A2> callable(R (*func)(A1 a1, A2 a2))
{ return Function<R, A1, A2>(func); }

/** Creates and returns a FunctionSlot object for the given free/static function. */
template <typename R, class A1, class A2>
FunctionSlot<R, A1, A2> slot( R (*func)(A1 a1, A2 a2) )
{ return FunctionSlot<R, A1, A2>( callable(func) ); }

/** Creates and returns a FunctionSlot object for the given std::function. */
template <typename R, class A1, class A2>
FunctionSlot<R, A1, A2> slot( const std::function<R(A1, A2)>& func )
{ return FunctionSlot<R, A1, A2>( func ); }

// END_Function 2
// BEGIN_Function 1
template <typename R, class A1>
class Function<R, A1, Void, Void, Void, Void, Void, Void, Void, Void, Void> : public Callable<R, A1, Void, Void, Void, Void, Void, Void, Void, Void, Void>
{
    public:
        /** The function signature wrapped by this class. */
        typedef std::function<R(A1)> FuncT;

        /** Wraps func. */
        Function(FuncT func)
        : _function(func) { }

        /** Deeply copies f. */
        Function(const Function& f) = default;

        /** Call the wrapped function, passing it the arguments as-is and returning
        its return value. */
        R operator()(A1 a1) const
        { return _function(a1); }

        /** Creates a clone of this object and returns it. The caller owns the returned object. */
        Function<R, A1>* clone() const
        { return new Function(*this); }

        /** Returns true if rhs and this object point to the same function. */
        bool operator==(const Function& rhs) const
        {
            return getAddress(_function) == getAddress(rhs._function);
        }

    private:
        FuncT _function;
};

/** Creates and returns a Function wrapper for the given free/static function. */
template <typename R, class A1>
Function<R, A1> callable(R (*func)(A1 a1))
{ return Function<R, A1>(func); }

/** Creates and returns a FunctionSlot object for the given free/static function. */
template <typename R, class A1>
FunctionSlot<R, A1> slot( R (*func)(A1 a1) )
{ return FunctionSlot<R, A1>( callable(func) ); }

/** Creates and returns a FunctionSlot object for the given std::function. */
template <typename R, class A1>
FunctionSlot<R, A1> slot( const std::function<R(A1)>& func )
{ return FunctionSlot<R, A1>( func ); }

// END_Function 1
// BEGIN_Function 0
template <typename R>
class Function<R, Void, Void, Void, Void, Void, Void, Void, Void, Void, Void> : public Callable<R, Void, Void, Void, Void, Void, Void, Void, Void, Void, Void>
{
    public:
        /** The function signature wrapped by this class. */
        typedef std::function<R()> FuncT;

        /** Wraps func. */
        Function(FuncT func)
        : _function(func) { }

        /** Deeply copies f. */
        Function(const Function& f) = default;

        /** Call the wrapped function, passing it the arguments as-is and returning
        its return value. */
        R operator()() const
        { return _function(); }

        /** Creates a clone of this object and returns it. The caller owns the returned object. */
        Function<R>* clone() const
        { return new Function(*this); }

        /** Returns true if rhs and this object point to the same function. */
        bool operator==(const Function& rhs) const
        {
            return getAddress(_function) == getAddress(rhs._function);
        }

    private:
        FuncT _function;
};

/** Creates and returns a Function wrapper for the given free/static function. */
template <typename R>
Function<R> callable(R (*func)())
{ return Function<R>(func); }

/** Creates and returns a FunctionSlot object for the given free/static function. */
template <typename R>
FunctionSlot<R> slot( R (*func)() )
{ return FunctionSlot<R>( callable(func) ); }

/** Creates and returns a FunctionSlot object for the given std::function. */
template <typename R>
FunctionSlot<R> slot( const std::function<R()>& func )
{ return FunctionSlot<R>( func ); }

// END_Function 0
