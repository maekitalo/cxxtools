// BEGIN_Delegate 10
/**
A Delegate is essentially a Signal with only a single target slot.
*/
template < typename R,class A1 = Void, class A2 = Void, class A3 = Void, class A4 = Void, class A5 = Void, class A6 = Void, class A7 = Void, class A8 = Void, class A9 = Void, class A10 = Void>
class Delegate : public DelegateBase
    {
        public:
            typedef Callable<R,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10> Callable;

        public:
            /** Does nothing. */
            Delegate()
            { }

            /** Deeply copies rhs. */
            Delegate(const Delegate& rhs)
            {
                DelegateBase::operator=(rhs);
            }

            /** Connects this object to the given slot and returns that Connection. */
            Connection connect(const BasicSlot<R,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>& slot)
            {
                return Connection(*this, slot.clone() );
            }

            /**
              Passes on all arguments to the connected slot and returns the return value
              of that slot. If no slot is connect then an exception is thrown.
            */
            inline R call(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10) const
            {
                if( !_target.valid() ) {
                    throw std::logic_error("Delegate<R,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>::call(): Delegate not connected");
                }
                const Callable* cb = static_cast<const Callable*>( _target.slot().callable() );
                return cb->call(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10);
            }

            /**
              Passes on all arguments to the connected slot and ignores the return value. If
              No slot is connected, the call is silently ignored.
            */
            inline void invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10) const
            {
                if( !_target.valid() ) {
                    return;
                }
                const Callable* cb = static_cast<const Callable*>( _target.slot().callable() );
                cb->call(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10);
            }

            /** Identical to call(...). */
            R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10) const
            { return this->call(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10); }
    };
/**
DelegateSlot wraps Deletegate object so that they can behave like Slots.
*/
template < typename R,class A1 = Void, class A2 = Void, class A3 = Void, class A4 = Void, class A5 = Void, class A6 = Void, class A7 = Void, class A8 = Void, class A9 = Void, class A10 = Void>
class DelegateSlot : public BasicSlot<R, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>
{
        public:
            /** Wraps the given Delegate. */
            DelegateSlot(Delegate<R, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>& delegate)
            : _method( delegate, &Delegate<R,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>::call )
            {}

            /** Creates a copy of this object and returns it. Caller owns the returned object. */
            BasicSlot<R,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>* clone() const
            { return new DelegateSlot(*this); }

            /** Returns a pointer to this object's internal Callable. */
            virtual const void* callable() const
            {
                return &_method;
            }

            virtual bool opened(const Connection& c)
            {
                Connectable& connectable = _method.object();
                return connectable.opened(c);
            }

            virtual void closed(const Connection& c)
            {
                Connectable& connectable = _method.object();
                connectable.closed(c);
            }

            virtual bool equals(const Slot& slot) const
            {
                const DelegateSlot* ds = dynamic_cast<const DelegateSlot*>(&slot);
                if(!ds)
                    return false;

                return _method == ds->_method;
            }

        private:
            mutable ConstMethod<R, Delegate<R, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>, A1,A2,A3,A4,A5,A6,A7,A8,A9,A10 > _method;
};

/** Creates and returns a DelegateSlot for the given Delegate. */
    template < typename R,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
    DelegateSlot<R,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10> slot( Delegate<R,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>& delegate )
    { return DelegateSlot<R,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>( delegate ); }

    /** Connect a Delegate to another Delegate.
    */
    template <typename R,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
    Connection connect(Delegate<R,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>& delegate, Delegate<R,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>& receiver)
    {
        return connect( delegate,  slot(receiver) );
    }

    /** Connect a Delegate to a Slot.
    */
    template <typename R,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
    Connection connect(Delegate<R,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>& delegate, const BasicSlot<R,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>& slot)
    {
        return Connection(delegate, slot.clone() );
    }


    /** Connect a Delegate to a function.
    */
    template <typename R,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
    Connection connect(Delegate<R,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>& delegate, R(*func)(A1,A2,A3,A4,A5,A6,A7,A8,A9,A10))
    {
        return connect( delegate, slot(func) );
    }


    /** Connect a Delegate to a member function.
    */
    template <typename R, class BaseT, class ClassT,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
    Connection connect(Delegate<R,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10>& delegate, BaseT& object, R(ClassT::*memFunc)(A1,A2,A3,A4,A5,A6,A7,A8,A9,A10))
    {
        return connect( delegate, slot(object, memFunc) );
    }

    /** Connect a Delegate to a const member function.
    */
    template <typename R, class ClassT,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
    Connection connect(Delegate<R>& delegate, ClassT& object, R(ClassT::*memFunc)(A1,A2,A3,A4,A5,A6,A7,A8,A9,A10) const)
    {
        return connect( delegate, slot(object, memFunc) );
    }


// END_Delegate
// BEGIN_Delegate 9
template < typename R,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
class Delegate<R, A1,A2,A3,A4,A5,A6,A7,A8,A9,Void> : public DelegateBase
    {
        public:
            typedef Callable<R,A1,A2,A3,A4,A5,A6,A7,A8,A9> Callable;

        public:
            /** Does nothing. */
            Delegate()
            { }

            /** Deeply copies rhs. */
            Delegate(const Delegate& rhs)
            {
                DelegateBase::operator=(rhs);
            }

            /** Connects this object to the given slot and returns that Connection. */
            Connection connect(const BasicSlot<R,A1,A2,A3,A4,A5,A6,A7,A8,A9>& slot)
            {
                return Connection(*this, slot.clone() );
            }

            /**
              Passes on all arguments to the connected slot and returns the return value
              of that slot. If no slot is connect then an exception is thrown.
            */
            inline R call(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9) const
            {
                if( !_target.valid() ) {
                    throw std::logic_error("Delegate<R,A1,A2,A3,A4,A5,A6,A7,A8,A9>::call(): Delegate not connected");
                }
                const Callable* cb = static_cast<const Callable*>( _target.slot().callable() );
                return cb->call(a1,a2,a3,a4,a5,a6,a7,a8,a9);
            }

            /**
              Passes on all arguments to the connected slot and ignores the return value. If
              No slot is connected, the call is silently ignored.
            */
            inline void invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9) const
            {
                if( !_target.valid() ) {
                    return;
                }
                const Callable* cb = static_cast<const Callable*>( _target.slot().callable() );
                cb->call(a1,a2,a3,a4,a5,a6,a7,a8,a9);
            }

            /** Identical to call(...). */
            R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9) const
            { return this->call(a1,a2,a3,a4,a5,a6,a7,a8,a9); }
    };
    /** Connect a Delegate to another Delegate.
    */
    template <typename R,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
    Connection connect(Delegate<R,A1,A2,A3,A4,A5,A6,A7,A8,A9>& delegate, Delegate<R,A1,A2,A3,A4,A5,A6,A7,A8,A9>& receiver)
    {
        return connect( delegate,  slot(receiver) );
    }

    /** Connect a Delegate to a Slot.
    */
    template <typename R,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
    Connection connect(Delegate<R,A1,A2,A3,A4,A5,A6,A7,A8,A9>& delegate, const BasicSlot<R,A1,A2,A3,A4,A5,A6,A7,A8,A9>& slot)
    {
        return Connection(delegate, slot.clone() );
    }


    /** Connect a Delegate to a function.
    */
    template <typename R,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
    Connection connect(Delegate<R,A1,A2,A3,A4,A5,A6,A7,A8,A9>& delegate, R(*func)(A1,A2,A3,A4,A5,A6,A7,A8,A9))
    {
        return connect( delegate, slot(func) );
    }


    /** Connect a Delegate to a member function.
    */
    template <typename R, class BaseT, class ClassT,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
    Connection connect(Delegate<R,A1,A2,A3,A4,A5,A6,A7,A8,A9>& delegate, BaseT& object, R(ClassT::*memFunc)(A1,A2,A3,A4,A5,A6,A7,A8,A9))
    {
        return connect( delegate, slot(object, memFunc) );
    }

    /** Connect a Delegate to a const member function.
    */
    template <typename R, class ClassT,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
    Connection connect(Delegate<R>& delegate, ClassT& object, R(ClassT::*memFunc)(A1,A2,A3,A4,A5,A6,A7,A8,A9) const)
    {
        return connect( delegate, slot(object, memFunc) );
    }


// END_Delegate
// BEGIN_Delegate 8
template < typename R,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
class Delegate<R, A1,A2,A3,A4,A5,A6,A7,A8,Void,Void> : public DelegateBase
    {
        public:
            typedef Callable<R,A1,A2,A3,A4,A5,A6,A7,A8> Callable;

        public:
            /** Does nothing. */
            Delegate()
            { }

            /** Deeply copies rhs. */
            Delegate(const Delegate& rhs)
            {
                DelegateBase::operator=(rhs);
            }

            /** Connects this object to the given slot and returns that Connection. */
            Connection connect(const BasicSlot<R,A1,A2,A3,A4,A5,A6,A7,A8>& slot)
            {
                return Connection(*this, slot.clone() );
            }

            /**
              Passes on all arguments to the connected slot and returns the return value
              of that slot. If no slot is connect then an exception is thrown.
            */
            inline R call(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8) const
            {
                if( !_target.valid() ) {
                    throw std::logic_error("Delegate<R,A1,A2,A3,A4,A5,A6,A7,A8>::call(): Delegate not connected");
                }
                const Callable* cb = static_cast<const Callable*>( _target.slot().callable() );
                return cb->call(a1,a2,a3,a4,a5,a6,a7,a8);
            }

            /**
              Passes on all arguments to the connected slot and ignores the return value. If
              No slot is connected, the call is silently ignored.
            */
            inline void invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8) const
            {
                if( !_target.valid() ) {
                    return;
                }
                const Callable* cb = static_cast<const Callable*>( _target.slot().callable() );
                cb->call(a1,a2,a3,a4,a5,a6,a7,a8);
            }

            /** Identical to call(...). */
            R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8) const
            { return this->call(a1,a2,a3,a4,a5,a6,a7,a8); }
    };
    /** Connect a Delegate to another Delegate.
    */
    template <typename R,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
    Connection connect(Delegate<R,A1,A2,A3,A4,A5,A6,A7,A8>& delegate, Delegate<R,A1,A2,A3,A4,A5,A6,A7,A8>& receiver)
    {
        return connect( delegate,  slot(receiver) );
    }

    /** Connect a Delegate to a Slot.
    */
    template <typename R,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
    Connection connect(Delegate<R,A1,A2,A3,A4,A5,A6,A7,A8>& delegate, const BasicSlot<R,A1,A2,A3,A4,A5,A6,A7,A8>& slot)
    {
        return Connection(delegate, slot.clone() );
    }


    /** Connect a Delegate to a function.
    */
    template <typename R,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
    Connection connect(Delegate<R,A1,A2,A3,A4,A5,A6,A7,A8>& delegate, R(*func)(A1,A2,A3,A4,A5,A6,A7,A8))
    {
        return connect( delegate, slot(func) );
    }


    /** Connect a Delegate to a member function.
    */
    template <typename R, class BaseT, class ClassT,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
    Connection connect(Delegate<R,A1,A2,A3,A4,A5,A6,A7,A8>& delegate, BaseT& object, R(ClassT::*memFunc)(A1,A2,A3,A4,A5,A6,A7,A8))
    {
        return connect( delegate, slot(object, memFunc) );
    }

    /** Connect a Delegate to a const member function.
    */
    template <typename R, class ClassT,class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
    Connection connect(Delegate<R>& delegate, ClassT& object, R(ClassT::*memFunc)(A1,A2,A3,A4,A5,A6,A7,A8) const)
    {
        return connect( delegate, slot(object, memFunc) );
    }


// END_Delegate
// BEGIN_Delegate 7
template < typename R,class A1, class A2, class A3, class A4, class A5, class A6, class A7>
class Delegate<R, A1,A2,A3,A4,A5,A6,A7,Void,Void,Void> : public DelegateBase
    {
        public:
            typedef Callable<R,A1,A2,A3,A4,A5,A6,A7> Callable;

        public:
            /** Does nothing. */
            Delegate()
            { }

            /** Deeply copies rhs. */
            Delegate(const Delegate& rhs)
            {
                DelegateBase::operator=(rhs);
            }

            /** Connects this object to the given slot and returns that Connection. */
            Connection connect(const BasicSlot<R,A1,A2,A3,A4,A5,A6,A7>& slot)
            {
                return Connection(*this, slot.clone() );
            }

            /**
              Passes on all arguments to the connected slot and returns the return value
              of that slot. If no slot is connect then an exception is thrown.
            */
            inline R call(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7) const
            {
                if( !_target.valid() ) {
                    throw std::logic_error("Delegate<R,A1,A2,A3,A4,A5,A6,A7>::call(): Delegate not connected");
                }
                const Callable* cb = static_cast<const Callable*>( _target.slot().callable() );
                return cb->call(a1,a2,a3,a4,a5,a6,a7);
            }

            /**
              Passes on all arguments to the connected slot and ignores the return value. If
              No slot is connected, the call is silently ignored.
            */
            inline void invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7) const
            {
                if( !_target.valid() ) {
                    return;
                }
                const Callable* cb = static_cast<const Callable*>( _target.slot().callable() );
                cb->call(a1,a2,a3,a4,a5,a6,a7);
            }

            /** Identical to call(...). */
            R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7) const
            { return this->call(a1,a2,a3,a4,a5,a6,a7); }
    };
    /** Connect a Delegate to another Delegate.
    */
    template <typename R,class A1, class A2, class A3, class A4, class A5, class A6, class A7>
    Connection connect(Delegate<R,A1,A2,A3,A4,A5,A6,A7>& delegate, Delegate<R,A1,A2,A3,A4,A5,A6,A7>& receiver)
    {
        return connect( delegate,  slot(receiver) );
    }

    /** Connect a Delegate to a Slot.
    */
    template <typename R,class A1, class A2, class A3, class A4, class A5, class A6, class A7>
    Connection connect(Delegate<R,A1,A2,A3,A4,A5,A6,A7>& delegate, const BasicSlot<R,A1,A2,A3,A4,A5,A6,A7>& slot)
    {
        return Connection(delegate, slot.clone() );
    }


    /** Connect a Delegate to a function.
    */
    template <typename R,class A1, class A2, class A3, class A4, class A5, class A6, class A7>
    Connection connect(Delegate<R,A1,A2,A3,A4,A5,A6,A7>& delegate, R(*func)(A1,A2,A3,A4,A5,A6,A7))
    {
        return connect( delegate, slot(func) );
    }


    /** Connect a Delegate to a member function.
    */
    template <typename R, class BaseT, class ClassT,class A1, class A2, class A3, class A4, class A5, class A6, class A7>
    Connection connect(Delegate<R,A1,A2,A3,A4,A5,A6,A7>& delegate, BaseT& object, R(ClassT::*memFunc)(A1,A2,A3,A4,A5,A6,A7))
    {
        return connect( delegate, slot(object, memFunc) );
    }

    /** Connect a Delegate to a const member function.
    */
    template <typename R, class ClassT,class A1, class A2, class A3, class A4, class A5, class A6, class A7>
    Connection connect(Delegate<R>& delegate, ClassT& object, R(ClassT::*memFunc)(A1,A2,A3,A4,A5,A6,A7) const)
    {
        return connect( delegate, slot(object, memFunc) );
    }


// END_Delegate
// BEGIN_Delegate 6
template < typename R,class A1, class A2, class A3, class A4, class A5, class A6>
class Delegate<R, A1,A2,A3,A4,A5,A6,Void,Void,Void,Void> : public DelegateBase
    {
        public:
            typedef Callable<R,A1,A2,A3,A4,A5,A6> Callable;

        public:
            /** Does nothing. */
            Delegate()
            { }

            /** Deeply copies rhs. */
            Delegate(const Delegate& rhs)
            {
                DelegateBase::operator=(rhs);
            }

            /** Connects this object to the given slot and returns that Connection. */
            Connection connect(const BasicSlot<R,A1,A2,A3,A4,A5,A6>& slot)
            {
                return Connection(*this, slot.clone() );
            }

            /**
              Passes on all arguments to the connected slot and returns the return value
              of that slot. If no slot is connect then an exception is thrown.
            */
            inline R call(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) const
            {
                if( !_target.valid() ) {
                    throw std::logic_error("Delegate<R,A1,A2,A3,A4,A5,A6>::call(): Delegate not connected");
                }
                const Callable* cb = static_cast<const Callable*>( _target.slot().callable() );
                return cb->call(a1,a2,a3,a4,a5,a6);
            }

            /**
              Passes on all arguments to the connected slot and ignores the return value. If
              No slot is connected, the call is silently ignored.
            */
            inline void invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) const
            {
                if( !_target.valid() ) {
                    return;
                }
                const Callable* cb = static_cast<const Callable*>( _target.slot().callable() );
                cb->call(a1,a2,a3,a4,a5,a6);
            }

            /** Identical to call(...). */
            R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) const
            { return this->call(a1,a2,a3,a4,a5,a6); }
    };
    /** Connect a Delegate to another Delegate.
    */
    template <typename R,class A1, class A2, class A3, class A4, class A5, class A6>
    Connection connect(Delegate<R,A1,A2,A3,A4,A5,A6>& delegate, Delegate<R,A1,A2,A3,A4,A5,A6>& receiver)
    {
        return connect( delegate,  slot(receiver) );
    }

    /** Connect a Delegate to a Slot.
    */
    template <typename R,class A1, class A2, class A3, class A4, class A5, class A6>
    Connection connect(Delegate<R,A1,A2,A3,A4,A5,A6>& delegate, const BasicSlot<R,A1,A2,A3,A4,A5,A6>& slot)
    {
        return Connection(delegate, slot.clone() );
    }


    /** Connect a Delegate to a function.
    */
    template <typename R,class A1, class A2, class A3, class A4, class A5, class A6>
    Connection connect(Delegate<R,A1,A2,A3,A4,A5,A6>& delegate, R(*func)(A1,A2,A3,A4,A5,A6))
    {
        return connect( delegate, slot(func) );
    }


    /** Connect a Delegate to a member function.
    */
    template <typename R, class BaseT, class ClassT,class A1, class A2, class A3, class A4, class A5, class A6>
    Connection connect(Delegate<R,A1,A2,A3,A4,A5,A6>& delegate, BaseT& object, R(ClassT::*memFunc)(A1,A2,A3,A4,A5,A6))
    {
        return connect( delegate, slot(object, memFunc) );
    }

    /** Connect a Delegate to a const member function.
    */
    template <typename R, class ClassT,class A1, class A2, class A3, class A4, class A5, class A6>
    Connection connect(Delegate<R>& delegate, ClassT& object, R(ClassT::*memFunc)(A1,A2,A3,A4,A5,A6) const)
    {
        return connect( delegate, slot(object, memFunc) );
    }


// END_Delegate
// BEGIN_Delegate 5
template < typename R,class A1, class A2, class A3, class A4, class A5>
class Delegate<R, A1,A2,A3,A4,A5,Void,Void,Void,Void,Void> : public DelegateBase
    {
        public:
            typedef Callable<R,A1,A2,A3,A4,A5> Callable;

        public:
            /** Does nothing. */
            Delegate()
            { }

            /** Deeply copies rhs. */
            Delegate(const Delegate& rhs)
            {
                DelegateBase::operator=(rhs);
            }

            /** Connects this object to the given slot and returns that Connection. */
            Connection connect(const BasicSlot<R,A1,A2,A3,A4,A5>& slot)
            {
                return Connection(*this, slot.clone() );
            }

            /**
              Passes on all arguments to the connected slot and returns the return value
              of that slot. If no slot is connect then an exception is thrown.
            */
            inline R call(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) const
            {
                if( !_target.valid() ) {
                    throw std::logic_error("Delegate<R,A1,A2,A3,A4,A5>::call(): Delegate not connected");
                }
                const Callable* cb = static_cast<const Callable*>( _target.slot().callable() );
                return cb->call(a1,a2,a3,a4,a5);
            }

            /**
              Passes on all arguments to the connected slot and ignores the return value. If
              No slot is connected, the call is silently ignored.
            */
            inline void invoke(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) const
            {
                if( !_target.valid() ) {
                    return;
                }
                const Callable* cb = static_cast<const Callable*>( _target.slot().callable() );
                cb->call(a1,a2,a3,a4,a5);
            }

            /** Identical to call(...). */
            R operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) const
            { return this->call(a1,a2,a3,a4,a5); }
    };
    /** Connect a Delegate to another Delegate.
    */
    template <typename R,class A1, class A2, class A3, class A4, class A5>
    Connection connect(Delegate<R,A1,A2,A3,A4,A5>& delegate, Delegate<R,A1,A2,A3,A4,A5>& receiver)
    {
        return connect( delegate,  slot(receiver) );
    }

    /** Connect a Delegate to a Slot.
    */
    template <typename R,class A1, class A2, class A3, class A4, class A5>
    Connection connect(Delegate<R,A1,A2,A3,A4,A5>& delegate, const BasicSlot<R,A1,A2,A3,A4,A5>& slot)
    {
        return Connection(delegate, slot.clone() );
    }


    /** Connect a Delegate to a function.
    */
    template <typename R,class A1, class A2, class A3, class A4, class A5>
    Connection connect(Delegate<R,A1,A2,A3,A4,A5>& delegate, R(*func)(A1,A2,A3,A4,A5))
    {
        return connect( delegate, slot(func) );
    }


    /** Connect a Delegate to a member function.
    */
    template <typename R, class BaseT, class ClassT,class A1, class A2, class A3, class A4, class A5>
    Connection connect(Delegate<R,A1,A2,A3,A4,A5>& delegate, BaseT& object, R(ClassT::*memFunc)(A1,A2,A3,A4,A5))
    {
        return connect( delegate, slot(object, memFunc) );
    }

    /** Connect a Delegate to a const member function.
    */
    template <typename R, class ClassT,class A1, class A2, class A3, class A4, class A5>
    Connection connect(Delegate<R>& delegate, ClassT& object, R(ClassT::*memFunc)(A1,A2,A3,A4,A5) const)
    {
        return connect( delegate, slot(object, memFunc) );
    }


// END_Delegate
// BEGIN_Delegate 4
template < typename R,class A1, class A2, class A3, class A4>
class Delegate<R, A1,A2,A3,A4,Void,Void,Void,Void,Void,Void> : public DelegateBase
    {
        public:
            typedef Callable<R,A1,A2,A3,A4> Callable;

        public:
            /** Does nothing. */
            Delegate()
            { }

            /** Deeply copies rhs. */
            Delegate(const Delegate& rhs)
            {
                DelegateBase::operator=(rhs);
            }

            /** Connects this object to the given slot and returns that Connection. */
            Connection connect(const BasicSlot<R,A1,A2,A3,A4>& slot)
            {
                return Connection(*this, slot.clone() );
            }

            /**
              Passes on all arguments to the connected slot and returns the return value
              of that slot. If no slot is connect then an exception is thrown.
            */
            inline R call(A1 a1, A2 a2, A3 a3, A4 a4) const
            {
                if( !_target.valid() ) {
                    throw std::logic_error("Delegate<R,A1,A2,A3,A4>::call(): Delegate not connected");
                }
                const Callable* cb = static_cast<const Callable*>( _target.slot().callable() );
                return cb->call(a1,a2,a3,a4);
            }

            /**
              Passes on all arguments to the connected slot and ignores the return value. If
              No slot is connected, the call is silently ignored.
            */
            inline void invoke(A1 a1, A2 a2, A3 a3, A4 a4) const
            {
                if( !_target.valid() ) {
                    return;
                }
                const Callable* cb = static_cast<const Callable*>( _target.slot().callable() );
                cb->call(a1,a2,a3,a4);
            }

            /** Identical to call(...). */
            R operator()(A1 a1, A2 a2, A3 a3, A4 a4) const
            { return this->call(a1,a2,a3,a4); }
    };
    /** Connect a Delegate to another Delegate.
    */
    template <typename R,class A1, class A2, class A3, class A4>
    Connection connect(Delegate<R,A1,A2,A3,A4>& delegate, Delegate<R,A1,A2,A3,A4>& receiver)
    {
        return connect( delegate,  slot(receiver) );
    }

    /** Connect a Delegate to a Slot.
    */
    template <typename R,class A1, class A2, class A3, class A4>
    Connection connect(Delegate<R,A1,A2,A3,A4>& delegate, const BasicSlot<R,A1,A2,A3,A4>& slot)
    {
        return Connection(delegate, slot.clone() );
    }


    /** Connect a Delegate to a function.
    */
    template <typename R,class A1, class A2, class A3, class A4>
    Connection connect(Delegate<R,A1,A2,A3,A4>& delegate, R(*func)(A1,A2,A3,A4))
    {
        return connect( delegate, slot(func) );
    }


    /** Connect a Delegate to a member function.
    */
    template <typename R, class BaseT, class ClassT,class A1, class A2, class A3, class A4>
    Connection connect(Delegate<R,A1,A2,A3,A4>& delegate, BaseT& object, R(ClassT::*memFunc)(A1,A2,A3,A4))
    {
        return connect( delegate, slot(object, memFunc) );
    }

    /** Connect a Delegate to a const member function.
    */
    template <typename R, class ClassT,class A1, class A2, class A3, class A4>
    Connection connect(Delegate<R>& delegate, ClassT& object, R(ClassT::*memFunc)(A1,A2,A3,A4) const)
    {
        return connect( delegate, slot(object, memFunc) );
    }


// END_Delegate
// BEGIN_Delegate 3
template < typename R,class A1, class A2, class A3>
class Delegate<R, A1,A2,A3,Void,Void,Void,Void,Void,Void,Void> : public DelegateBase
    {
        public:
            typedef Callable<R,A1,A2,A3> Callable;

        public:
            /** Does nothing. */
            Delegate()
            { }

            /** Deeply copies rhs. */
            Delegate(const Delegate& rhs)
            {
                DelegateBase::operator=(rhs);
            }

            /** Connects this object to the given slot and returns that Connection. */
            Connection connect(const BasicSlot<R,A1,A2,A3>& slot)
            {
                return Connection(*this, slot.clone() );
            }

            /**
              Passes on all arguments to the connected slot and returns the return value
              of that slot. If no slot is connect then an exception is thrown.
            */
            inline R call(A1 a1, A2 a2, A3 a3) const
            {
                if( !_target.valid() ) {
                    throw std::logic_error("Delegate<R,A1,A2,A3>::call(): Delegate not connected");
                }
                const Callable* cb = static_cast<const Callable*>( _target.slot().callable() );
                return cb->call(a1,a2,a3);
            }

            /**
              Passes on all arguments to the connected slot and ignores the return value. If
              No slot is connected, the call is silently ignored.
            */
            inline void invoke(A1 a1, A2 a2, A3 a3) const
            {
                if( !_target.valid() ) {
                    return;
                }
                const Callable* cb = static_cast<const Callable*>( _target.slot().callable() );
                cb->call(a1,a2,a3);
            }

            /** Identical to call(...). */
            R operator()(A1 a1, A2 a2, A3 a3) const
            { return this->call(a1,a2,a3); }
    };
    /** Connect a Delegate to another Delegate.
    */
    template <typename R,class A1, class A2, class A3>
    Connection connect(Delegate<R,A1,A2,A3>& delegate, Delegate<R,A1,A2,A3>& receiver)
    {
        return connect( delegate,  slot(receiver) );
    }

    /** Connect a Delegate to a Slot.
    */
    template <typename R,class A1, class A2, class A3>
    Connection connect(Delegate<R,A1,A2,A3>& delegate, const BasicSlot<R,A1,A2,A3>& slot)
    {
        return Connection(delegate, slot.clone() );
    }


    /** Connect a Delegate to a function.
    */
    template <typename R,class A1, class A2, class A3>
    Connection connect(Delegate<R,A1,A2,A3>& delegate, R(*func)(A1,A2,A3))
    {
        return connect( delegate, slot(func) );
    }


    /** Connect a Delegate to a member function.
    */
    template <typename R, class BaseT, class ClassT,class A1, class A2, class A3>
    Connection connect(Delegate<R,A1,A2,A3>& delegate, BaseT& object, R(ClassT::*memFunc)(A1,A2,A3))
    {
        return connect( delegate, slot(object, memFunc) );
    }

    /** Connect a Delegate to a const member function.
    */
    template <typename R, class ClassT,class A1, class A2, class A3>
    Connection connect(Delegate<R>& delegate, ClassT& object, R(ClassT::*memFunc)(A1,A2,A3) const)
    {
        return connect( delegate, slot(object, memFunc) );
    }


// END_Delegate
// BEGIN_Delegate 2
template < typename R,class A1, class A2>
class Delegate<R, A1,A2,Void,Void,Void,Void,Void,Void,Void,Void> : public DelegateBase
    {
        public:
            typedef Callable<R,A1,A2> Callable;

        public:
            /** Does nothing. */
            Delegate()
            { }

            /** Deeply copies rhs. */
            Delegate(const Delegate& rhs)
            {
                DelegateBase::operator=(rhs);
            }

            /** Connects this object to the given slot and returns that Connection. */
            Connection connect(const BasicSlot<R,A1,A2>& slot)
            {
                return Connection(*this, slot.clone() );
            }

            /**
              Passes on all arguments to the connected slot and returns the return value
              of that slot. If no slot is connect then an exception is thrown.
            */
            inline R call(A1 a1, A2 a2) const
            {
                if( !_target.valid() ) {
                    throw std::logic_error("Delegate<R,A1,A2>::call(): Delegate not connected");
                }
                const Callable* cb = static_cast<const Callable*>( _target.slot().callable() );
                return cb->call(a1,a2);
            }

            /**
              Passes on all arguments to the connected slot and ignores the return value. If
              No slot is connected, the call is silently ignored.
            */
            inline void invoke(A1 a1, A2 a2) const
            {
                if( !_target.valid() ) {
                    return;
                }
                const Callable* cb = static_cast<const Callable*>( _target.slot().callable() );
                cb->call(a1,a2);
            }

            /** Identical to call(...). */
            R operator()(A1 a1, A2 a2) const
            { return this->call(a1,a2); }
    };
    /** Connect a Delegate to another Delegate.
    */
    template <typename R,class A1, class A2>
    Connection connect(Delegate<R,A1,A2>& delegate, Delegate<R,A1,A2>& receiver)
    {
        return connect( delegate,  slot(receiver) );
    }

    /** Connect a Delegate to a Slot.
    */
    template <typename R,class A1, class A2>
    Connection connect(Delegate<R,A1,A2>& delegate, const BasicSlot<R,A1,A2>& slot)
    {
        return Connection(delegate, slot.clone() );
    }


    /** Connect a Delegate to a function.
    */
    template <typename R,class A1, class A2>
    Connection connect(Delegate<R,A1,A2>& delegate, R(*func)(A1,A2))
    {
        return connect( delegate, slot(func) );
    }


    /** Connect a Delegate to a member function.
    */
    template <typename R, class BaseT, class ClassT,class A1, class A2>
    Connection connect(Delegate<R,A1,A2>& delegate, BaseT& object, R(ClassT::*memFunc)(A1,A2))
    {
        return connect( delegate, slot(object, memFunc) );
    }

    /** Connect a Delegate to a const member function.
    */
    template <typename R, class ClassT,class A1, class A2>
    Connection connect(Delegate<R>& delegate, ClassT& object, R(ClassT::*memFunc)(A1,A2) const)
    {
        return connect( delegate, slot(object, memFunc) );
    }


// END_Delegate
// BEGIN_Delegate 1
template < typename R,class A1>
class Delegate<R, A1,Void,Void,Void,Void,Void,Void,Void,Void,Void> : public DelegateBase
    {
        public:
            typedef Callable<R,A1> Callable;

        public:
            /** Does nothing. */
            Delegate()
            { }

            /** Deeply copies rhs. */
            Delegate(const Delegate& rhs)
            {
                DelegateBase::operator=(rhs);
            }

            /** Connects this object to the given slot and returns that Connection. */
            Connection connect(const BasicSlot<R,A1>& slot)
            {
                return Connection(*this, slot.clone() );
            }

            /**
              Passes on all arguments to the connected slot and returns the return value
              of that slot. If no slot is connect then an exception is thrown.
            */
            inline R call(A1 a1) const
            {
                if( !_target.valid() ) {
                    throw std::logic_error("Delegate<R,A1>::call(): Delegate not connected");
                }
                const Callable* cb = static_cast<const Callable*>( _target.slot().callable() );
                return cb->call(a1);
            }

            /**
              Passes on all arguments to the connected slot and ignores the return value. If
              No slot is connected, the call is silently ignored.
            */
            inline void invoke(A1 a1) const
            {
                if( !_target.valid() ) {
                    return;
                }
                const Callable* cb = static_cast<const Callable*>( _target.slot().callable() );
                cb->call(a1);
            }

            /** Identical to call(...). */
            R operator()(A1 a1) const
            { return this->call(a1); }
    };
    /** Connect a Delegate to another Delegate.
    */
    template <typename R,class A1>
    Connection connect(Delegate<R,A1>& delegate, Delegate<R,A1>& receiver)
    {
        return connect( delegate,  slot(receiver) );
    }

    /** Connect a Delegate to a Slot.
    */
    template <typename R,class A1>
    Connection connect(Delegate<R,A1>& delegate, const BasicSlot<R,A1>& slot)
    {
        return Connection(delegate, slot.clone() );
    }


    /** Connect a Delegate to a function.
    */
    template <typename R,class A1>
    Connection connect(Delegate<R,A1>& delegate, R(*func)(A1))
    {
        return connect( delegate, slot(func) );
    }


    /** Connect a Delegate to a member function.
    */
    template <typename R, class BaseT, class ClassT,class A1>
    Connection connect(Delegate<R,A1>& delegate, BaseT& object, R(ClassT::*memFunc)(A1))
    {
        return connect( delegate, slot(object, memFunc) );
    }

    /** Connect a Delegate to a const member function.
    */
    template <typename R, class ClassT,class A1>
    Connection connect(Delegate<R>& delegate, ClassT& object, R(ClassT::*memFunc)(A1) const)
    {
        return connect( delegate, slot(object, memFunc) );
    }


// END_Delegate
// BEGIN_Delegate 0
template < typename R>
class Delegate<R, Void,Void,Void,Void,Void,Void,Void,Void,Void,Void> : public DelegateBase
    {
        public:
            typedef Callable<R> Callable;

        public:
            /** Does nothing. */
            Delegate()
            { }

            /** Deeply copies rhs. */
            Delegate(const Delegate& rhs)
            {
                DelegateBase::operator=(rhs);
            }

            /** Connects this object to the given slot and returns that Connection. */
            Connection connect(const BasicSlot<R>& slot)
            {
                return Connection(*this, slot.clone() );
            }

            /**
              Passes on all arguments to the connected slot and returns the return value
              of that slot. If no slot is connect then an exception is thrown.
            */
            inline R call() const
            {
                if( !_target.valid() ) {
                    throw std::logic_error("Delegate<R>::call(): Delegate not connected");
                }
                const Callable* cb = static_cast<const Callable*>( _target.slot().callable() );
                return cb->call();
            }

            /**
              Passes on all arguments to the connected slot and ignores the return value. If
              No slot is connected, the call is silently ignored.
            */
            inline void invoke() const
            {
                if( !_target.valid() ) {
                    return;
                }
                const Callable* cb = static_cast<const Callable*>( _target.slot().callable() );
                cb->call();
            }

            /** Identical to call(...). */
            R operator()() const
            { return this->call(); }
    };
    /** Connect a Delegate to another Delegate.
    */
    template <typename R>
    Connection connect(Delegate<R>& delegate, Delegate<R>& receiver)
    {
        return connect( delegate,  slot(receiver) );
    }

    /** Connect a Delegate to a Slot.
    */
    template <typename R>
    Connection connect(Delegate<R>& delegate, const BasicSlot<R>& slot)
    {
        return Connection(delegate, slot.clone() );
    }


    /** Connect a Delegate to a function.
    */
    template <typename R>
    Connection connect(Delegate<R>& delegate, R(*func)())
    {
        return connect( delegate, slot(func) );
    }


    /** Connect a Delegate to a member function.
    */
    template <typename R, class BaseT, class ClassT>
    Connection connect(Delegate<R>& delegate, BaseT& object, R(ClassT::*memFunc)())
    {
        return connect( delegate, slot(object, memFunc) );
    }

    /** Connect a Delegate to a const member function.
    */
    template <typename R, class ClassT>
    Connection connect(Delegate<R>& delegate, ClassT& object, R(ClassT::*memFunc)() const)
    {
        return connect( delegate, slot(object, memFunc) );
    }


// END_Delegate
