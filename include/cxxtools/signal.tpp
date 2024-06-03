// BEGIN_Signal 10
    /** Multicast Signal

        A Signal can be connected to multiple targets. The return
        value of the target(s) is/are ignored.
    */
    template <class A1 = Void, class A2 = Void, class A3 = Void, class A4 = Void, class A5 = Void, class A6 = Void, class A7 = Void, class A8 = Void, class A9 = Void, class A10 = Void>
    class Signal : public SignalBase {
        public:
            typedef Invokable<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10> InvokableT;

        public:
            /**
            Connects slot to this signal, such that firing this signal
            will invoke slot.
            */
            template <typename R>
            Connection connect(const BasicSlot<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>& slot)
            {
                return Connection(*this, slot.clone() );
            }

            /** The converse of connect(). */
            template <typename R>
            void disconnect(const BasicSlot<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>& slot)
            {
                this->disconnectSlot(slot);
            }

            /**
            Invokes all slots connected to this signal, in an undefined
            order. Their return values are ignored. Calling of connected slots will
            be interrupted if a slot deletes this Signal object or throws an exception.
            */
            inline void send(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10) const
            {
                // The sentry will set the Signal to the sending state and
                // reset it to not-sending upon destruction. In the sending
                // state, removing connection will leave invalid connections
                // in the connection list to keep the iterator valid, but mark
                // the Signal dirty. If the Signal is dirty, all invalid
                // connections will be removed by the Sentry when it destructs..
                SignalBase::Sentry sentry(this);

                std::list<Connection>::const_iterator it = Connectable::connections().begin();
                std::list<Connection>::const_iterator end = Connectable::connections().end();

                for(; it != end; ++it)
                {
                    if( false == it->valid() || &( it->sender() ) != this  )
                        continue;

                    // The following scenarios must be considered when the
                    // slot is called:
                    // - The slot might get deleted and thus disconnected from
                    //   this signal
                    // - The slot might delete this signal and we must end
                    //   calling any slots immediately
                    // - A new Connection might get added to this Signal in
                    //   the slot
                    const InvokableT* invokable = static_cast<const InvokableT*>( it->slot().callable() );
                    invokable->invoke(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10);

                    // if this signal gets deleted by the slot, the Sentry
                    // will be detached. In this case we bail out immediately
                    if( !sentry )
                        return;
                }
            }

            /** Same as send(...). */
            inline void operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, A10 a10) const
            { this->send(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10); }
    };

// END_Signal 10
// BEGIN_SignalSlot 10
    /**
      SignalSlot is a "slot wrapper" for Signal objects. That is, it
      effectively converts a Signal object into a Slot object, so that it
      can be used as the target of another Signal. This allows chaining of
      Signals.
    */
    template <class A1 = Void, class A2 = Void, class A3 = Void, class A4 = Void, class A5 = Void, class A6 = Void, class A7 = Void, class A8 = Void, class A9 = Void, class A10 = Void>
    class SignalSlot : public BasicSlot<void, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>
    {
        public:
            /** Wraps signal. */
            SignalSlot(Signal<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>& signal)
            : _method( signal, &Signal<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>::send )
            {}

            /** Creates a clone of this object and returns it. Caller owns
            the returned object. */
            Slot* clone() const
            { return new SignalSlot(*this); }

            /** Returns a pointer to this object's internal Callable object. */
            virtual const void* callable() const
            {
                return &_method;
            }

            /** ??? */
            virtual void onConnect(const Connection& c)
            {
                _method.object().onConnectionOpen(c);
            }

            /** ??? */
            virtual void onDisconnect(const Connection& c)
            {
                _method.object().onConnectionClose(c);
            }
            /** returns true if this object and rhs are equivalent. */
            virtual bool equals(const Slot& rhs) const
            {
                const SignalSlot* ss = dynamic_cast<const SignalSlot*>(&rhs);
                return ss ? (_method == ss->_method) : false;
            }

        private:
            mutable ConstMethod<void, Signal<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10 > _method;
    };

    /** Creates a SignalSlot object from an equivalent Signal. */
    template <class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
    SignalSlot<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10> slot( Signal<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10> & signal )
    { return SignalSlot<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>( signal ); }


    /** Connects the given signal and slot objects and returns that Connection
    object (which can normally be ignored). */
    template <typename R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
    Connection connect(Signal<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>& signal, const BasicSlot<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>& slot)
    {
        return signal.connect( slot );
    }




    //! Connects a Signal to a function.
    template <typename R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
    Connection connect(Signal<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>& signal, R(*func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10))
    {
        return connect( signal, slot(func) );
    }

    //! Connects a Signal to a member function.
    template <typename R, class BaseT, typename ClassT, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
    Connection connect(Signal<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>& signal, BaseT& object, R(ClassT::*memFunc)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10))
    {
        return connect( signal, slot(object, memFunc) );
    }

    //! Connects a Signal to a const member function.
    template <typename R, class BaseT, typename ClassT, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
    Connection connect(Signal<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>& signal, BaseT& object, R(ClassT::*memFunc)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) const)
    {
        return connect( signal, slot(object, memFunc) );
    }

    /// Connects a Signal to another Signal
/**
    template <class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
    Connection connect(Signal<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>& sender, Signal<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>& receiver)
    {
        return connect( sender, slot(receiver) );
    }
*/

    template <typename R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
    void disconnect(Signal<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>& signal, const BasicSlot<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>& slot)
    {
        return signal.disconnect( slot );
    }

    template <typename R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
    void disconnect(Signal<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>& signal, R(*func)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10))
    {
        signal.disconnect( slot(func) );
    }

    template <typename R, typename BaseT, typename ClassT, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
    void disconnect(Signal<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>& signal, BaseT & object, R(ClassT::*memFunc)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10))
    {
        signal.disconnect( slot( object, memFunc ) );
    }

    template <typename R, class BaseT, typename ClassT, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
    void disconnect(Signal<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>& signal, BaseT& object, R(ClassT::*memFunc)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) const)
    {
        signal.disconnect( slot( object, memFunc ) );
    }

    template <typename R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
    void disconnect( Signal<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>& sender, Signal<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>& receiver )
    {
        sender.disconnect( slot(receiver) );
    }

    /** Connects a Signal to another Signal. */
    template <class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
    Connection connect(Signal<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>& sender, Signal<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>& receiver)
    {
        return connect( sender, slot(receiver) );
    }
// END_SignalSlot 10
// BEGIN_Signal 9
    // specialization
    template <class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
    class Signal<A1, A2, A3, A4, A5, A6, A7, A8, A9, Void> : public SignalBase {
        public:
            typedef Invokable<A1, A2, A3, A4, A5, A6, A7, A8, A9, Void> InvokableT;

        public:
            /**
            Connects slot to this signal, such that firing this signal
            will invoke slot.
            */
            template <typename R>
            Connection connect(const BasicSlot<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, Void>& slot)
            {
                return Connection(*this, slot.clone() );
            }

            /** The converse of connect(). */
            template <typename R>
            void disconnect(const BasicSlot<R, A1, A2, A3, A4, A5, A6, A7, A8, A9, Void>& slot)
            {
                this->disconnectSlot(slot);
            }

            /**
            Invokes all slots connected to this signal, in an undefined
            order. Their return values are ignored. Calling of connected slots will
            be interrupted if a slot deletes this Signal object or throws an exception.
            */
            inline void send(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9) const
            {
                // The sentry will set the Signal to the sending state and
                // reset it to not-sending upon destruction. In the sending
                // state, removing connection will leave invalid connections
                // in the connection list to keep the iterator valid, but mark
                // the Signal dirty. If the Signal is dirty, all invalid
                // connections will be removed by the Sentry when it destructs..
                SignalBase::Sentry sentry(this);

                std::list<Connection>::const_iterator it = Connectable::connections().begin();
                std::list<Connection>::const_iterator end = Connectable::connections().end();

                for(; it != end; ++it)
                {
                    if( false == it->valid() || &( it->sender() ) != this  )
                        continue;

                    // The following scenarios must be considered when the
                    // slot is called:
                    // - The slot might get deleted and thus disconnected from
                    //   this signal
                    // - The slot might delete this signal and we must end
                    //   calling any slots immediately
                    // - A new Connection might get added to this Signal in
                    //   the slot
                    const InvokableT* invokable = static_cast<const InvokableT*>( it->slot().callable() );
                    invokable->invoke(a1, a2, a3, a4, a5, a6, a7, a8, a9);

                    // if this signal gets deleted by the slot, the Sentry
                    // will be detached. In this case we bail out immediately
                    if( !sentry )
                        return;
                }
            }

            /** Same as send(...). */
            inline void operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9) const
            { this->send(a1, a2, a3, a4, a5, a6, a7, a8, a9); }
    };

// END_Signal 9
// BEGIN_SignalSlot 9

    //! Connects a Signal to a function.
    template <typename R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
    Connection connect(Signal<A1, A2, A3, A4, A5, A6, A7, A8, A9>& signal, R(*func)(A1, A2, A3, A4, A5, A6, A7, A8, A9))
    {
        return connect( signal, slot(func) );
    }

    //! Connects a Signal to a member function.
    template <typename R, class BaseT, typename ClassT, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
    Connection connect(Signal<A1, A2, A3, A4, A5, A6, A7, A8, A9>& signal, BaseT& object, R(ClassT::*memFunc)(A1, A2, A3, A4, A5, A6, A7, A8, A9))
    {
        return connect( signal, slot(object, memFunc) );
    }

    //! Connects a Signal to a const member function.
    template <typename R, class BaseT, typename ClassT, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
    Connection connect(Signal<A1, A2, A3, A4, A5, A6, A7, A8, A9>& signal, BaseT& object, R(ClassT::*memFunc)(A1, A2, A3, A4, A5, A6, A7, A8, A9) const)
    {
        return connect( signal, slot(object, memFunc) );
    }

    /// Connects a Signal to another Signal
/**
    template <class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
    Connection connect(Signal<A1, A2, A3, A4, A5, A6, A7, A8, A9>& sender, Signal<A1, A2, A3, A4, A5, A6, A7, A8, A9>& receiver)
    {
        return connect( sender, slot(receiver) );
    }
*/

    template <typename R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
    void disconnect(Signal<A1, A2, A3, A4, A5, A6, A7, A8, A9>& signal, R(*func)(A1, A2, A3, A4, A5, A6, A7, A8, A9))
    {
        signal.disconnect( slot(func) );
    }

    template <typename R, typename BaseT, typename ClassT, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
    void disconnect(Signal<A1, A2, A3, A4, A5, A6, A7, A8, A9>& signal, BaseT & object, R(ClassT::*memFunc)(A1, A2, A3, A4, A5, A6, A7, A8, A9))
    {
        signal.disconnect( slot( object, memFunc ) );
    }

    template <typename R, class BaseT, typename ClassT, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
    void disconnect(Signal<A1, A2, A3, A4, A5, A6, A7, A8, A9>& signal, BaseT& object, R(ClassT::*memFunc)(A1, A2, A3, A4, A5, A6, A7, A8, A9) const)
    {
        signal.disconnect( slot( object, memFunc ) );
    }

    template <typename R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
    void disconnect( Signal<A1, A2, A3, A4, A5, A6, A7, A8, A9>& sender, Signal<A1, A2, A3, A4, A5, A6, A7, A8, A9>& receiver )
    {
        sender.disconnect( slot(receiver) );
    }

    /** Connects a Signal to another Signal. */
    template <class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
    Connection connect(Signal<A1, A2, A3, A4, A5, A6, A7, A8, A9>& sender, Signal<A1, A2, A3, A4, A5, A6, A7, A8, A9>& receiver)
    {
        return connect( sender, slot(receiver) );
    }
// END_SignalSlot 9
// BEGIN_Signal 8
    // specialization
    template <class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
    class Signal<A1, A2, A3, A4, A5, A6, A7, A8, Void, Void> : public SignalBase {
        public:
            typedef Invokable<A1, A2, A3, A4, A5, A6, A7, A8, Void, Void> InvokableT;

        public:
            /**
            Connects slot to this signal, such that firing this signal
            will invoke slot.
            */
            template <typename R>
            Connection connect(const BasicSlot<R, A1, A2, A3, A4, A5, A6, A7, A8, Void, Void>& slot)
            {
                return Connection(*this, slot.clone() );
            }

            /** The converse of connect(). */
            template <typename R>
            void disconnect(const BasicSlot<R, A1, A2, A3, A4, A5, A6, A7, A8, Void, Void>& slot)
            {
                this->disconnectSlot(slot);
            }

            /**
            Invokes all slots connected to this signal, in an undefined
            order. Their return values are ignored. Calling of connected slots will
            be interrupted if a slot deletes this Signal object or throws an exception.
            */
            inline void send(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8) const
            {
                // The sentry will set the Signal to the sending state and
                // reset it to not-sending upon destruction. In the sending
                // state, removing connection will leave invalid connections
                // in the connection list to keep the iterator valid, but mark
                // the Signal dirty. If the Signal is dirty, all invalid
                // connections will be removed by the Sentry when it destructs..
                SignalBase::Sentry sentry(this);

                std::list<Connection>::const_iterator it = Connectable::connections().begin();
                std::list<Connection>::const_iterator end = Connectable::connections().end();

                for(; it != end; ++it)
                {
                    if( false == it->valid() || &( it->sender() ) != this  )
                        continue;

                    // The following scenarios must be considered when the
                    // slot is called:
                    // - The slot might get deleted and thus disconnected from
                    //   this signal
                    // - The slot might delete this signal and we must end
                    //   calling any slots immediately
                    // - A new Connection might get added to this Signal in
                    //   the slot
                    const InvokableT* invokable = static_cast<const InvokableT*>( it->slot().callable() );
                    invokable->invoke(a1, a2, a3, a4, a5, a6, a7, a8);

                    // if this signal gets deleted by the slot, the Sentry
                    // will be detached. In this case we bail out immediately
                    if( !sentry )
                        return;
                }
            }

            /** Same as send(...). */
            inline void operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8) const
            { this->send(a1, a2, a3, a4, a5, a6, a7, a8); }
    };

// END_Signal 8
// BEGIN_SignalSlot 8

    //! Connects a Signal to a function.
    template <typename R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
    Connection connect(Signal<A1, A2, A3, A4, A5, A6, A7, A8>& signal, R(*func)(A1, A2, A3, A4, A5, A6, A7, A8))
    {
        return connect( signal, slot(func) );
    }

    //! Connects a Signal to a member function.
    template <typename R, class BaseT, typename ClassT, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
    Connection connect(Signal<A1, A2, A3, A4, A5, A6, A7, A8>& signal, BaseT& object, R(ClassT::*memFunc)(A1, A2, A3, A4, A5, A6, A7, A8))
    {
        return connect( signal, slot(object, memFunc) );
    }

    //! Connects a Signal to a const member function.
    template <typename R, class BaseT, typename ClassT, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
    Connection connect(Signal<A1, A2, A3, A4, A5, A6, A7, A8>& signal, BaseT& object, R(ClassT::*memFunc)(A1, A2, A3, A4, A5, A6, A7, A8) const)
    {
        return connect( signal, slot(object, memFunc) );
    }

    /// Connects a Signal to another Signal
/**
    template <class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
    Connection connect(Signal<A1, A2, A3, A4, A5, A6, A7, A8>& sender, Signal<A1, A2, A3, A4, A5, A6, A7, A8>& receiver)
    {
        return connect( sender, slot(receiver) );
    }
*/

    template <typename R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
    void disconnect(Signal<A1, A2, A3, A4, A5, A6, A7, A8>& signal, R(*func)(A1, A2, A3, A4, A5, A6, A7, A8))
    {
        signal.disconnect( slot(func) );
    }

    template <typename R, typename BaseT, typename ClassT, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
    void disconnect(Signal<A1, A2, A3, A4, A5, A6, A7, A8>& signal, BaseT & object, R(ClassT::*memFunc)(A1, A2, A3, A4, A5, A6, A7, A8))
    {
        signal.disconnect( slot( object, memFunc ) );
    }

    template <typename R, class BaseT, typename ClassT, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
    void disconnect(Signal<A1, A2, A3, A4, A5, A6, A7, A8>& signal, BaseT& object, R(ClassT::*memFunc)(A1, A2, A3, A4, A5, A6, A7, A8) const)
    {
        signal.disconnect( slot( object, memFunc ) );
    }

    template <typename R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
    void disconnect( Signal<A1, A2, A3, A4, A5, A6, A7, A8>& sender, Signal<A1, A2, A3, A4, A5, A6, A7, A8>& receiver )
    {
        sender.disconnect( slot(receiver) );
    }

    /** Connects a Signal to another Signal. */
    template <class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
    Connection connect(Signal<A1, A2, A3, A4, A5, A6, A7, A8>& sender, Signal<A1, A2, A3, A4, A5, A6, A7, A8>& receiver)
    {
        return connect( sender, slot(receiver) );
    }
// END_SignalSlot 8
// BEGIN_Signal 7
    // specialization
    template <class A1, class A2, class A3, class A4, class A5, class A6, class A7>
    class Signal<A1, A2, A3, A4, A5, A6, A7, Void, Void, Void> : public SignalBase {
        public:
            typedef Invokable<A1, A2, A3, A4, A5, A6, A7, Void, Void, Void> InvokableT;

        public:
            /**
            Connects slot to this signal, such that firing this signal
            will invoke slot.
            */
            template <typename R>
            Connection connect(const BasicSlot<R, A1, A2, A3, A4, A5, A6, A7, Void, Void, Void>& slot)
            {
                return Connection(*this, slot.clone() );
            }

            /** The converse of connect(). */
            template <typename R>
            void disconnect(const BasicSlot<R, A1, A2, A3, A4, A5, A6, A7, Void, Void, Void>& slot)
            {
                this->disconnectSlot(slot);
            }

            /**
            Invokes all slots connected to this signal, in an undefined
            order. Their return values are ignored. Calling of connected slots will
            be interrupted if a slot deletes this Signal object or throws an exception.
            */
            inline void send(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7) const
            {
                // The sentry will set the Signal to the sending state and
                // reset it to not-sending upon destruction. In the sending
                // state, removing connection will leave invalid connections
                // in the connection list to keep the iterator valid, but mark
                // the Signal dirty. If the Signal is dirty, all invalid
                // connections will be removed by the Sentry when it destructs..
                SignalBase::Sentry sentry(this);

                std::list<Connection>::const_iterator it = Connectable::connections().begin();
                std::list<Connection>::const_iterator end = Connectable::connections().end();

                for(; it != end; ++it)
                {
                    if( false == it->valid() || &( it->sender() ) != this  )
                        continue;

                    // The following scenarios must be considered when the
                    // slot is called:
                    // - The slot might get deleted and thus disconnected from
                    //   this signal
                    // - The slot might delete this signal and we must end
                    //   calling any slots immediately
                    // - A new Connection might get added to this Signal in
                    //   the slot
                    const InvokableT* invokable = static_cast<const InvokableT*>( it->slot().callable() );
                    invokable->invoke(a1, a2, a3, a4, a5, a6, a7);

                    // if this signal gets deleted by the slot, the Sentry
                    // will be detached. In this case we bail out immediately
                    if( !sentry )
                        return;
                }
            }

            /** Same as send(...). */
            inline void operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7) const
            { this->send(a1, a2, a3, a4, a5, a6, a7); }
    };

// END_Signal 7
// BEGIN_SignalSlot 7

    //! Connects a Signal to a function.
    template <typename R, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
    Connection connect(Signal<A1, A2, A3, A4, A5, A6, A7>& signal, R(*func)(A1, A2, A3, A4, A5, A6, A7))
    {
        return connect( signal, slot(func) );
    }

    //! Connects a Signal to a member function.
    template <typename R, class BaseT, typename ClassT, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
    Connection connect(Signal<A1, A2, A3, A4, A5, A6, A7>& signal, BaseT& object, R(ClassT::*memFunc)(A1, A2, A3, A4, A5, A6, A7))
    {
        return connect( signal, slot(object, memFunc) );
    }

    //! Connects a Signal to a const member function.
    template <typename R, class BaseT, typename ClassT, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
    Connection connect(Signal<A1, A2, A3, A4, A5, A6, A7>& signal, BaseT& object, R(ClassT::*memFunc)(A1, A2, A3, A4, A5, A6, A7) const)
    {
        return connect( signal, slot(object, memFunc) );
    }

    /// Connects a Signal to another Signal
/**
    template <class A1, class A2, class A3, class A4, class A5, class A6, class A7>
    Connection connect(Signal<A1, A2, A3, A4, A5, A6, A7>& sender, Signal<A1, A2, A3, A4, A5, A6, A7>& receiver)
    {
        return connect( sender, slot(receiver) );
    }
*/

    template <typename R, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
    void disconnect(Signal<A1, A2, A3, A4, A5, A6, A7>& signal, R(*func)(A1, A2, A3, A4, A5, A6, A7))
    {
        signal.disconnect( slot(func) );
    }

    template <typename R, typename BaseT, typename ClassT, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
    void disconnect(Signal<A1, A2, A3, A4, A5, A6, A7>& signal, BaseT & object, R(ClassT::*memFunc)(A1, A2, A3, A4, A5, A6, A7))
    {
        signal.disconnect( slot( object, memFunc ) );
    }

    template <typename R, class BaseT, typename ClassT, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
    void disconnect(Signal<A1, A2, A3, A4, A5, A6, A7>& signal, BaseT& object, R(ClassT::*memFunc)(A1, A2, A3, A4, A5, A6, A7) const)
    {
        signal.disconnect( slot( object, memFunc ) );
    }

    template <typename R, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
    void disconnect( Signal<A1, A2, A3, A4, A5, A6, A7>& sender, Signal<A1, A2, A3, A4, A5, A6, A7>& receiver )
    {
        sender.disconnect( slot(receiver) );
    }

    /** Connects a Signal to another Signal. */
    template <class A1, class A2, class A3, class A4, class A5, class A6, class A7>
    Connection connect(Signal<A1, A2, A3, A4, A5, A6, A7>& sender, Signal<A1, A2, A3, A4, A5, A6, A7>& receiver)
    {
        return connect( sender, slot(receiver) );
    }
// END_SignalSlot 7
// BEGIN_Signal 6
    // specialization
    template <class A1, class A2, class A3, class A4, class A5, class A6>
    class Signal<A1, A2, A3, A4, A5, A6, Void, Void, Void, Void> : public SignalBase {
        public:
            typedef Invokable<A1, A2, A3, A4, A5, A6, Void, Void, Void, Void> InvokableT;

        public:
            /**
            Connects slot to this signal, such that firing this signal
            will invoke slot.
            */
            template <typename R>
            Connection connect(const BasicSlot<R, A1, A2, A3, A4, A5, A6, Void, Void, Void, Void>& slot)
            {
                return Connection(*this, slot.clone() );
            }

            /** The converse of connect(). */
            template <typename R>
            void disconnect(const BasicSlot<R, A1, A2, A3, A4, A5, A6, Void, Void, Void, Void>& slot)
            {
                this->disconnectSlot(slot);
            }

            /**
            Invokes all slots connected to this signal, in an undefined
            order. Their return values are ignored. Calling of connected slots will
            be interrupted if a slot deletes this Signal object or throws an exception.
            */
            inline void send(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) const
            {
                // The sentry will set the Signal to the sending state and
                // reset it to not-sending upon destruction. In the sending
                // state, removing connection will leave invalid connections
                // in the connection list to keep the iterator valid, but mark
                // the Signal dirty. If the Signal is dirty, all invalid
                // connections will be removed by the Sentry when it destructs..
                SignalBase::Sentry sentry(this);

                std::list<Connection>::const_iterator it = Connectable::connections().begin();
                std::list<Connection>::const_iterator end = Connectable::connections().end();

                for(; it != end; ++it)
                {
                    if( false == it->valid() || &( it->sender() ) != this  )
                        continue;

                    // The following scenarios must be considered when the
                    // slot is called:
                    // - The slot might get deleted and thus disconnected from
                    //   this signal
                    // - The slot might delete this signal and we must end
                    //   calling any slots immediately
                    // - A new Connection might get added to this Signal in
                    //   the slot
                    const InvokableT* invokable = static_cast<const InvokableT*>( it->slot().callable() );
                    invokable->invoke(a1, a2, a3, a4, a5, a6);

                    // if this signal gets deleted by the slot, the Sentry
                    // will be detached. In this case we bail out immediately
                    if( !sentry )
                        return;
                }
            }

            /** Same as send(...). */
            inline void operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6) const
            { this->send(a1, a2, a3, a4, a5, a6); }
    };

// END_Signal 6
// BEGIN_SignalSlot 6

    //! Connects a Signal to a function.
    template <typename R, class A1, class A2, class A3, class A4, class A5, class A6>
    Connection connect(Signal<A1, A2, A3, A4, A5, A6>& signal, R(*func)(A1, A2, A3, A4, A5, A6))
    {
        return connect( signal, slot(func) );
    }

    //! Connects a Signal to a member function.
    template <typename R, class BaseT, typename ClassT, class A1, class A2, class A3, class A4, class A5, class A6>
    Connection connect(Signal<A1, A2, A3, A4, A5, A6>& signal, BaseT& object, R(ClassT::*memFunc)(A1, A2, A3, A4, A5, A6))
    {
        return connect( signal, slot(object, memFunc) );
    }

    //! Connects a Signal to a const member function.
    template <typename R, class BaseT, typename ClassT, class A1, class A2, class A3, class A4, class A5, class A6>
    Connection connect(Signal<A1, A2, A3, A4, A5, A6>& signal, BaseT& object, R(ClassT::*memFunc)(A1, A2, A3, A4, A5, A6) const)
    {
        return connect( signal, slot(object, memFunc) );
    }

    /// Connects a Signal to another Signal
/**
    template <class A1, class A2, class A3, class A4, class A5, class A6>
    Connection connect(Signal<A1, A2, A3, A4, A5, A6>& sender, Signal<A1, A2, A3, A4, A5, A6>& receiver)
    {
        return connect( sender, slot(receiver) );
    }
*/

    template <typename R, class A1, class A2, class A3, class A4, class A5, class A6>
    void disconnect(Signal<A1, A2, A3, A4, A5, A6>& signal, R(*func)(A1, A2, A3, A4, A5, A6))
    {
        signal.disconnect( slot(func) );
    }

    template <typename R, typename BaseT, typename ClassT, class A1, class A2, class A3, class A4, class A5, class A6>
    void disconnect(Signal<A1, A2, A3, A4, A5, A6>& signal, BaseT & object, R(ClassT::*memFunc)(A1, A2, A3, A4, A5, A6))
    {
        signal.disconnect( slot( object, memFunc ) );
    }

    template <typename R, class BaseT, typename ClassT, class A1, class A2, class A3, class A4, class A5, class A6>
    void disconnect(Signal<A1, A2, A3, A4, A5, A6>& signal, BaseT& object, R(ClassT::*memFunc)(A1, A2, A3, A4, A5, A6) const)
    {
        signal.disconnect( slot( object, memFunc ) );
    }

    template <typename R, class A1, class A2, class A3, class A4, class A5, class A6>
    void disconnect( Signal<A1, A2, A3, A4, A5, A6>& sender, Signal<A1, A2, A3, A4, A5, A6>& receiver )
    {
        sender.disconnect( slot(receiver) );
    }

    /** Connects a Signal to another Signal. */
    template <class A1, class A2, class A3, class A4, class A5, class A6>
    Connection connect(Signal<A1, A2, A3, A4, A5, A6>& sender, Signal<A1, A2, A3, A4, A5, A6>& receiver)
    {
        return connect( sender, slot(receiver) );
    }
// END_SignalSlot 6
// BEGIN_Signal 5
    // specialization
    template <class A1, class A2, class A3, class A4, class A5>
    class Signal<A1, A2, A3, A4, A5, Void, Void, Void, Void, Void> : public SignalBase {
        public:
            typedef Invokable<A1, A2, A3, A4, A5, Void, Void, Void, Void, Void> InvokableT;

        public:
            /**
            Connects slot to this signal, such that firing this signal
            will invoke slot.
            */
            template <typename R>
            Connection connect(const BasicSlot<R, A1, A2, A3, A4, A5, Void, Void, Void, Void, Void>& slot)
            {
                return Connection(*this, slot.clone() );
            }

            /** The converse of connect(). */
            template <typename R>
            void disconnect(const BasicSlot<R, A1, A2, A3, A4, A5, Void, Void, Void, Void, Void>& slot)
            {
                this->disconnectSlot(slot);
            }

            /**
            Invokes all slots connected to this signal, in an undefined
            order. Their return values are ignored. Calling of connected slots will
            be interrupted if a slot deletes this Signal object or throws an exception.
            */
            inline void send(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) const
            {
                // The sentry will set the Signal to the sending state and
                // reset it to not-sending upon destruction. In the sending
                // state, removing connection will leave invalid connections
                // in the connection list to keep the iterator valid, but mark
                // the Signal dirty. If the Signal is dirty, all invalid
                // connections will be removed by the Sentry when it destructs..
                SignalBase::Sentry sentry(this);

                std::list<Connection>::const_iterator it = Connectable::connections().begin();
                std::list<Connection>::const_iterator end = Connectable::connections().end();

                for(; it != end; ++it)
                {
                    if( false == it->valid() || &( it->sender() ) != this  )
                        continue;

                    // The following scenarios must be considered when the
                    // slot is called:
                    // - The slot might get deleted and thus disconnected from
                    //   this signal
                    // - The slot might delete this signal and we must end
                    //   calling any slots immediately
                    // - A new Connection might get added to this Signal in
                    //   the slot
                    const InvokableT* invokable = static_cast<const InvokableT*>( it->slot().callable() );
                    invokable->invoke(a1, a2, a3, a4, a5);

                    // if this signal gets deleted by the slot, the Sentry
                    // will be detached. In this case we bail out immediately
                    if( !sentry )
                        return;
                }
            }

            /** Same as send(...). */
            inline void operator()(A1 a1, A2 a2, A3 a3, A4 a4, A5 a5) const
            { this->send(a1, a2, a3, a4, a5); }
    };

// END_Signal 5
// BEGIN_SignalSlot 5

    //! Connects a Signal to a function.
    template <typename R, class A1, class A2, class A3, class A4, class A5>
    Connection connect(Signal<A1, A2, A3, A4, A5>& signal, R(*func)(A1, A2, A3, A4, A5))
    {
        return connect( signal, slot(func) );
    }

    //! Connects a Signal to a member function.
    template <typename R, class BaseT, typename ClassT, class A1, class A2, class A3, class A4, class A5>
    Connection connect(Signal<A1, A2, A3, A4, A5>& signal, BaseT& object, R(ClassT::*memFunc)(A1, A2, A3, A4, A5))
    {
        return connect( signal, slot(object, memFunc) );
    }

    //! Connects a Signal to a const member function.
    template <typename R, class BaseT, typename ClassT, class A1, class A2, class A3, class A4, class A5>
    Connection connect(Signal<A1, A2, A3, A4, A5>& signal, BaseT& object, R(ClassT::*memFunc)(A1, A2, A3, A4, A5) const)
    {
        return connect( signal, slot(object, memFunc) );
    }

    /// Connects a Signal to another Signal
/**
    template <class A1, class A2, class A3, class A4, class A5>
    Connection connect(Signal<A1, A2, A3, A4, A5>& sender, Signal<A1, A2, A3, A4, A5>& receiver)
    {
        return connect( sender, slot(receiver) );
    }
*/

    template <typename R, class A1, class A2, class A3, class A4, class A5>
    void disconnect(Signal<A1, A2, A3, A4, A5>& signal, R(*func)(A1, A2, A3, A4, A5))
    {
        signal.disconnect( slot(func) );
    }

    template <typename R, typename BaseT, typename ClassT, class A1, class A2, class A3, class A4, class A5>
    void disconnect(Signal<A1, A2, A3, A4, A5>& signal, BaseT & object, R(ClassT::*memFunc)(A1, A2, A3, A4, A5))
    {
        signal.disconnect( slot( object, memFunc ) );
    }

    template <typename R, class BaseT, typename ClassT, class A1, class A2, class A3, class A4, class A5>
    void disconnect(Signal<A1, A2, A3, A4, A5>& signal, BaseT& object, R(ClassT::*memFunc)(A1, A2, A3, A4, A5) const)
    {
        signal.disconnect( slot( object, memFunc ) );
    }

    template <typename R, class A1, class A2, class A3, class A4, class A5>
    void disconnect( Signal<A1, A2, A3, A4, A5>& sender, Signal<A1, A2, A3, A4, A5>& receiver )
    {
        sender.disconnect( slot(receiver) );
    }

    /** Connects a Signal to another Signal. */
    template <class A1, class A2, class A3, class A4, class A5>
    Connection connect(Signal<A1, A2, A3, A4, A5>& sender, Signal<A1, A2, A3, A4, A5>& receiver)
    {
        return connect( sender, slot(receiver) );
    }
// END_SignalSlot 5
// BEGIN_Signal 4
    // specialization
    template <class A1, class A2, class A3, class A4>
    class Signal<A1, A2, A3, A4, Void, Void, Void, Void, Void, Void> : public SignalBase {
        public:
            typedef Invokable<A1, A2, A3, A4, Void, Void, Void, Void, Void, Void> InvokableT;

        public:
            /**
            Connects slot to this signal, such that firing this signal
            will invoke slot.
            */
            template <typename R>
            Connection connect(const BasicSlot<R, A1, A2, A3, A4, Void, Void, Void, Void, Void, Void>& slot)
            {
                return Connection(*this, slot.clone() );
            }

            /** The converse of connect(). */
            template <typename R>
            void disconnect(const BasicSlot<R, A1, A2, A3, A4, Void, Void, Void, Void, Void, Void>& slot)
            {
                this->disconnectSlot(slot);
            }

            /**
            Invokes all slots connected to this signal, in an undefined
            order. Their return values are ignored. Calling of connected slots will
            be interrupted if a slot deletes this Signal object or throws an exception.
            */
            inline void send(A1 a1, A2 a2, A3 a3, A4 a4) const
            {
                // The sentry will set the Signal to the sending state and
                // reset it to not-sending upon destruction. In the sending
                // state, removing connection will leave invalid connections
                // in the connection list to keep the iterator valid, but mark
                // the Signal dirty. If the Signal is dirty, all invalid
                // connections will be removed by the Sentry when it destructs..
                SignalBase::Sentry sentry(this);

                std::list<Connection>::const_iterator it = Connectable::connections().begin();
                std::list<Connection>::const_iterator end = Connectable::connections().end();

                for(; it != end; ++it)
                {
                    if( false == it->valid() || &( it->sender() ) != this  )
                        continue;

                    // The following scenarios must be considered when the
                    // slot is called:
                    // - The slot might get deleted and thus disconnected from
                    //   this signal
                    // - The slot might delete this signal and we must end
                    //   calling any slots immediately
                    // - A new Connection might get added to this Signal in
                    //   the slot
                    const InvokableT* invokable = static_cast<const InvokableT*>( it->slot().callable() );
                    invokable->invoke(a1, a2, a3, a4);

                    // if this signal gets deleted by the slot, the Sentry
                    // will be detached. In this case we bail out immediately
                    if( !sentry )
                        return;
                }
            }

            /** Same as send(...). */
            inline void operator()(A1 a1, A2 a2, A3 a3, A4 a4) const
            { this->send(a1, a2, a3, a4); }
    };

// END_Signal 4
// BEGIN_SignalSlot 4

    //! Connects a Signal to a function.
    template <typename R, class A1, class A2, class A3, class A4>
    Connection connect(Signal<A1, A2, A3, A4>& signal, R(*func)(A1, A2, A3, A4))
    {
        return connect( signal, slot(func) );
    }

    //! Connects a Signal to a member function.
    template <typename R, class BaseT, typename ClassT, class A1, class A2, class A3, class A4>
    Connection connect(Signal<A1, A2, A3, A4>& signal, BaseT& object, R(ClassT::*memFunc)(A1, A2, A3, A4))
    {
        return connect( signal, slot(object, memFunc) );
    }

    //! Connects a Signal to a const member function.
    template <typename R, class BaseT, typename ClassT, class A1, class A2, class A3, class A4>
    Connection connect(Signal<A1, A2, A3, A4>& signal, BaseT& object, R(ClassT::*memFunc)(A1, A2, A3, A4) const)
    {
        return connect( signal, slot(object, memFunc) );
    }

    /// Connects a Signal to another Signal
/**
    template <class A1, class A2, class A3, class A4>
    Connection connect(Signal<A1, A2, A3, A4>& sender, Signal<A1, A2, A3, A4>& receiver)
    {
        return connect( sender, slot(receiver) );
    }
*/

    template <typename R, class A1, class A2, class A3, class A4>
    void disconnect(Signal<A1, A2, A3, A4>& signal, R(*func)(A1, A2, A3, A4))
    {
        signal.disconnect( slot(func) );
    }

    template <typename R, typename BaseT, typename ClassT, class A1, class A2, class A3, class A4>
    void disconnect(Signal<A1, A2, A3, A4>& signal, BaseT & object, R(ClassT::*memFunc)(A1, A2, A3, A4))
    {
        signal.disconnect( slot( object, memFunc ) );
    }

    template <typename R, class BaseT, typename ClassT, class A1, class A2, class A3, class A4>
    void disconnect(Signal<A1, A2, A3, A4>& signal, BaseT& object, R(ClassT::*memFunc)(A1, A2, A3, A4) const)
    {
        signal.disconnect( slot( object, memFunc ) );
    }

    template <typename R, class A1, class A2, class A3, class A4>
    void disconnect( Signal<A1, A2, A3, A4>& sender, Signal<A1, A2, A3, A4>& receiver )
    {
        sender.disconnect( slot(receiver) );
    }

    /** Connects a Signal to another Signal. */
    template <class A1, class A2, class A3, class A4>
    Connection connect(Signal<A1, A2, A3, A4>& sender, Signal<A1, A2, A3, A4>& receiver)
    {
        return connect( sender, slot(receiver) );
    }
// END_SignalSlot 4
// BEGIN_Signal 3
    // specialization
    template <class A1, class A2, class A3>
    class Signal<A1, A2, A3, Void, Void, Void, Void, Void, Void, Void> : public SignalBase {
        public:
            typedef Invokable<A1, A2, A3, Void, Void, Void, Void, Void, Void, Void> InvokableT;

        public:
            /**
            Connects slot to this signal, such that firing this signal
            will invoke slot.
            */
            template <typename R>
            Connection connect(const BasicSlot<R, A1, A2, A3, Void, Void, Void, Void, Void, Void, Void>& slot)
            {
                return Connection(*this, slot.clone() );
            }

            /** The converse of connect(). */
            template <typename R>
            void disconnect(const BasicSlot<R, A1, A2, A3, Void, Void, Void, Void, Void, Void, Void>& slot)
            {
                this->disconnectSlot(slot);
            }

            /**
            Invokes all slots connected to this signal, in an undefined
            order. Their return values are ignored. Calling of connected slots will
            be interrupted if a slot deletes this Signal object or throws an exception.
            */
            inline void send(A1 a1, A2 a2, A3 a3) const
            {
                // The sentry will set the Signal to the sending state and
                // reset it to not-sending upon destruction. In the sending
                // state, removing connection will leave invalid connections
                // in the connection list to keep the iterator valid, but mark
                // the Signal dirty. If the Signal is dirty, all invalid
                // connections will be removed by the Sentry when it destructs..
                SignalBase::Sentry sentry(this);

                std::list<Connection>::const_iterator it = Connectable::connections().begin();
                std::list<Connection>::const_iterator end = Connectable::connections().end();

                for(; it != end; ++it)
                {
                    if( false == it->valid() || &( it->sender() ) != this  )
                        continue;

                    // The following scenarios must be considered when the
                    // slot is called:
                    // - The slot might get deleted and thus disconnected from
                    //   this signal
                    // - The slot might delete this signal and we must end
                    //   calling any slots immediately
                    // - A new Connection might get added to this Signal in
                    //   the slot
                    const InvokableT* invokable = static_cast<const InvokableT*>( it->slot().callable() );
                    invokable->invoke(a1, a2, a3);

                    // if this signal gets deleted by the slot, the Sentry
                    // will be detached. In this case we bail out immediately
                    if( !sentry )
                        return;
                }
            }

            /** Same as send(...). */
            inline void operator()(A1 a1, A2 a2, A3 a3) const
            { this->send(a1, a2, a3); }
    };

// END_Signal 3
// BEGIN_SignalSlot 3

    //! Connects a Signal to a function.
    template <typename R, class A1, class A2, class A3>
    Connection connect(Signal<A1, A2, A3>& signal, R(*func)(A1, A2, A3))
    {
        return connect( signal, slot(func) );
    }

    //! Connects a Signal to a member function.
    template <typename R, class BaseT, typename ClassT, class A1, class A2, class A3>
    Connection connect(Signal<A1, A2, A3>& signal, BaseT& object, R(ClassT::*memFunc)(A1, A2, A3))
    {
        return connect( signal, slot(object, memFunc) );
    }

    //! Connects a Signal to a const member function.
    template <typename R, class BaseT, typename ClassT, class A1, class A2, class A3>
    Connection connect(Signal<A1, A2, A3>& signal, BaseT& object, R(ClassT::*memFunc)(A1, A2, A3) const)
    {
        return connect( signal, slot(object, memFunc) );
    }

    /// Connects a Signal to another Signal
/**
    template <class A1, class A2, class A3>
    Connection connect(Signal<A1, A2, A3>& sender, Signal<A1, A2, A3>& receiver)
    {
        return connect( sender, slot(receiver) );
    }
*/

    template <typename R, class A1, class A2, class A3>
    void disconnect(Signal<A1, A2, A3>& signal, R(*func)(A1, A2, A3))
    {
        signal.disconnect( slot(func) );
    }

    template <typename R, typename BaseT, typename ClassT, class A1, class A2, class A3>
    void disconnect(Signal<A1, A2, A3>& signal, BaseT & object, R(ClassT::*memFunc)(A1, A2, A3))
    {
        signal.disconnect( slot( object, memFunc ) );
    }

    template <typename R, class BaseT, typename ClassT, class A1, class A2, class A3>
    void disconnect(Signal<A1, A2, A3>& signal, BaseT& object, R(ClassT::*memFunc)(A1, A2, A3) const)
    {
        signal.disconnect( slot( object, memFunc ) );
    }

    template <typename R, class A1, class A2, class A3>
    void disconnect( Signal<A1, A2, A3>& sender, Signal<A1, A2, A3>& receiver )
    {
        sender.disconnect( slot(receiver) );
    }

    /** Connects a Signal to another Signal. */
    template <class A1, class A2, class A3>
    Connection connect(Signal<A1, A2, A3>& sender, Signal<A1, A2, A3>& receiver)
    {
        return connect( sender, slot(receiver) );
    }
// END_SignalSlot 3
// BEGIN_Signal 2
    // specialization
    template <class A1, class A2>
    class Signal<A1, A2, Void, Void, Void, Void, Void, Void, Void, Void> : public SignalBase {
        public:
            typedef Invokable<A1, A2, Void, Void, Void, Void, Void, Void, Void, Void> InvokableT;

        public:
            /**
            Connects slot to this signal, such that firing this signal
            will invoke slot.
            */
            template <typename R>
            Connection connect(const BasicSlot<R, A1, A2, Void, Void, Void, Void, Void, Void, Void, Void>& slot)
            {
                return Connection(*this, slot.clone() );
            }

            /** The converse of connect(). */
            template <typename R>
            void disconnect(const BasicSlot<R, A1, A2, Void, Void, Void, Void, Void, Void, Void, Void>& slot)
            {
                this->disconnectSlot(slot);
            }

            /**
            Invokes all slots connected to this signal, in an undefined
            order. Their return values are ignored. Calling of connected slots will
            be interrupted if a slot deletes this Signal object or throws an exception.
            */
            inline void send(A1 a1, A2 a2) const
            {
                // The sentry will set the Signal to the sending state and
                // reset it to not-sending upon destruction. In the sending
                // state, removing connection will leave invalid connections
                // in the connection list to keep the iterator valid, but mark
                // the Signal dirty. If the Signal is dirty, all invalid
                // connections will be removed by the Sentry when it destructs..
                SignalBase::Sentry sentry(this);

                std::list<Connection>::const_iterator it = Connectable::connections().begin();
                std::list<Connection>::const_iterator end = Connectable::connections().end();

                for(; it != end; ++it)
                {
                    if( false == it->valid() || &( it->sender() ) != this  )
                        continue;

                    // The following scenarios must be considered when the
                    // slot is called:
                    // - The slot might get deleted and thus disconnected from
                    //   this signal
                    // - The slot might delete this signal and we must end
                    //   calling any slots immediately
                    // - A new Connection might get added to this Signal in
                    //   the slot
                    const InvokableT* invokable = static_cast<const InvokableT*>( it->slot().callable() );
                    invokable->invoke(a1, a2);

                    // if this signal gets deleted by the slot, the Sentry
                    // will be detached. In this case we bail out immediately
                    if( !sentry )
                        return;
                }
            }

            /** Same as send(...). */
            inline void operator()(A1 a1, A2 a2) const
            { this->send(a1, a2); }
    };

// END_Signal 2
// BEGIN_SignalSlot 2

    //! Connects a Signal to a function.
    template <typename R, class A1, class A2>
    Connection connect(Signal<A1, A2>& signal, R(*func)(A1, A2))
    {
        return connect( signal, slot(func) );
    }

    //! Connects a Signal to a member function.
    template <typename R, class BaseT, typename ClassT, class A1, class A2>
    Connection connect(Signal<A1, A2>& signal, BaseT& object, R(ClassT::*memFunc)(A1, A2))
    {
        return connect( signal, slot(object, memFunc) );
    }

    //! Connects a Signal to a const member function.
    template <typename R, class BaseT, typename ClassT, class A1, class A2>
    Connection connect(Signal<A1, A2>& signal, BaseT& object, R(ClassT::*memFunc)(A1, A2) const)
    {
        return connect( signal, slot(object, memFunc) );
    }

    /// Connects a Signal to another Signal
/**
    template <class A1, class A2>
    Connection connect(Signal<A1, A2>& sender, Signal<A1, A2>& receiver)
    {
        return connect( sender, slot(receiver) );
    }
*/

    template <typename R, class A1, class A2>
    void disconnect(Signal<A1, A2>& signal, R(*func)(A1, A2))
    {
        signal.disconnect( slot(func) );
    }

    template <typename R, typename BaseT, typename ClassT, class A1, class A2>
    void disconnect(Signal<A1, A2>& signal, BaseT & object, R(ClassT::*memFunc)(A1, A2))
    {
        signal.disconnect( slot( object, memFunc ) );
    }

    template <typename R, class BaseT, typename ClassT, class A1, class A2>
    void disconnect(Signal<A1, A2>& signal, BaseT& object, R(ClassT::*memFunc)(A1, A2) const)
    {
        signal.disconnect( slot( object, memFunc ) );
    }

    template <typename R, class A1, class A2>
    void disconnect( Signal<A1, A2>& sender, Signal<A1, A2>& receiver )
    {
        sender.disconnect( slot(receiver) );
    }

    /** Connects a Signal to another Signal. */
    template <class A1, class A2>
    Connection connect(Signal<A1, A2>& sender, Signal<A1, A2>& receiver)
    {
        return connect( sender, slot(receiver) );
    }
// END_SignalSlot 2
// BEGIN_Signal 1
    // specialization
    template <class A1>
    class Signal<A1, Void, Void, Void, Void, Void, Void, Void, Void, Void> : public SignalBase {
        public:
            typedef Invokable<A1, Void, Void, Void, Void, Void, Void, Void, Void, Void> InvokableT;

        public:
            /**
            Connects slot to this signal, such that firing this signal
            will invoke slot.
            */
            template <typename R>
            Connection connect(const BasicSlot<R, A1, Void, Void, Void, Void, Void, Void, Void, Void, Void>& slot)
            {
                return Connection(*this, slot.clone() );
            }

            /** The converse of connect(). */
            template <typename R>
            void disconnect(const BasicSlot<R, A1, Void, Void, Void, Void, Void, Void, Void, Void, Void>& slot)
            {
                this->disconnectSlot(slot);
            }

            /**
            Invokes all slots connected to this signal, in an undefined
            order. Their return values are ignored. Calling of connected slots will
            be interrupted if a slot deletes this Signal object or throws an exception.
            */
            inline void send(A1 a1) const
            {
                // The sentry will set the Signal to the sending state and
                // reset it to not-sending upon destruction. In the sending
                // state, removing connection will leave invalid connections
                // in the connection list to keep the iterator valid, but mark
                // the Signal dirty. If the Signal is dirty, all invalid
                // connections will be removed by the Sentry when it destructs..
                SignalBase::Sentry sentry(this);

                std::list<Connection>::const_iterator it = Connectable::connections().begin();
                std::list<Connection>::const_iterator end = Connectable::connections().end();

                for(; it != end; ++it)
                {
                    if( false == it->valid() || &( it->sender() ) != this  )
                        continue;

                    // The following scenarios must be considered when the
                    // slot is called:
                    // - The slot might get deleted and thus disconnected from
                    //   this signal
                    // - The slot might delete this signal and we must end
                    //   calling any slots immediately
                    // - A new Connection might get added to this Signal in
                    //   the slot
                    const InvokableT* invokable = static_cast<const InvokableT*>( it->slot().callable() );
                    invokable->invoke(a1);

                    // if this signal gets deleted by the slot, the Sentry
                    // will be detached. In this case we bail out immediately
                    if( !sentry )
                        return;
                }
            }

            /** Same as send(...). */
            inline void operator()(A1 a1) const
            { this->send(a1); }
    };

// END_Signal 1
// BEGIN_SignalSlot 1

    //! Connects a Signal to a function.
    template <typename R, class A1>
    Connection connect(Signal<A1>& signal, R(*func)(A1))
    {
        return connect( signal, slot(func) );
    }

    //! Connects a Signal to a member function.
    template <typename R, class BaseT, typename ClassT, class A1>
    Connection connect(Signal<A1>& signal, BaseT& object, R(ClassT::*memFunc)(A1))
    {
        return connect( signal, slot(object, memFunc) );
    }

    //! Connects a Signal to a const member function.
    template <typename R, class BaseT, typename ClassT, class A1>
    Connection connect(Signal<A1>& signal, BaseT& object, R(ClassT::*memFunc)(A1) const)
    {
        return connect( signal, slot(object, memFunc) );
    }

    /// Connects a Signal to another Signal
/**
    template <class A1>
    Connection connect(Signal<A1>& sender, Signal<A1>& receiver)
    {
        return connect( sender, slot(receiver) );
    }
*/

    template <typename R, class A1>
    void disconnect(Signal<A1>& signal, R(*func)(A1))
    {
        signal.disconnect( slot(func) );
    }

    template <typename R, typename BaseT, typename ClassT, class A1>
    void disconnect(Signal<A1>& signal, BaseT & object, R(ClassT::*memFunc)(A1))
    {
        signal.disconnect( slot( object, memFunc ) );
    }

    template <typename R, class BaseT, typename ClassT, class A1>
    void disconnect(Signal<A1>& signal, BaseT& object, R(ClassT::*memFunc)(A1) const)
    {
        signal.disconnect( slot( object, memFunc ) );
    }

    template <typename R, class A1>
    void disconnect( Signal<A1>& sender, Signal<A1>& receiver )
    {
        sender.disconnect( slot(receiver) );
    }

    /** Connects a Signal to another Signal. */
    template <class A1>
    Connection connect(Signal<A1>& sender, Signal<A1>& receiver)
    {
        return connect( sender, slot(receiver) );
    }
// END_SignalSlot 1
// BEGIN_Signal 0
    // specialization
    template <>
    class Signal<Void, Void, Void, Void, Void, Void, Void, Void, Void, Void> : public SignalBase {
        public:
            typedef Invokable<Void, Void, Void, Void, Void, Void, Void, Void, Void, Void> InvokableT;

        public:
            /**
            Connects slot to this signal, such that firing this signal
            will invoke slot.
            */
            template <typename R>
            Connection connect(const BasicSlot<R, Void, Void, Void, Void, Void, Void, Void, Void, Void, Void>& slot)
            {
                return Connection(*this, slot.clone() );
            }

            /** The converse of connect(). */
            template <typename R>
            void disconnect(const BasicSlot<R, Void, Void, Void, Void, Void, Void, Void, Void, Void, Void>& slot)
            {
                this->disconnectSlot(slot);
            }

            /**
            Invokes all slots connected to this signal, in an undefined
            order. Their return values are ignored. Calling of connected slots will
            be interrupted if a slot deletes this Signal object or throws an exception.
            */
            inline void send() const
            {
                // The sentry will set the Signal to the sending state and
                // reset it to not-sending upon destruction. In the sending
                // state, removing connection will leave invalid connections
                // in the connection list to keep the iterator valid, but mark
                // the Signal dirty. If the Signal is dirty, all invalid
                // connections will be removed by the Sentry when it destructs..
                SignalBase::Sentry sentry(this);

                std::list<Connection>::const_iterator it = Connectable::connections().begin();
                std::list<Connection>::const_iterator end = Connectable::connections().end();

                for(; it != end; ++it)
                {
                    if( false == it->valid() || &( it->sender() ) != this  )
                        continue;

                    // The following scenarios must be considered when the
                    // slot is called:
                    // - The slot might get deleted and thus disconnected from
                    //   this signal
                    // - The slot might delete this signal and we must end
                    //   calling any slots immediately
                    // - A new Connection might get added to this Signal in
                    //   the slot
                    const InvokableT* invokable = static_cast<const InvokableT*>( it->slot().callable() );
                    invokable->invoke();

                    // if this signal gets deleted by the slot, the Sentry
                    // will be detached. In this case we bail out immediately
                    if( !sentry )
                        return;
                }
            }

            /** Same as send(...). */
            inline void operator()() const
            { this->send(); }
    };

// END_Signal 0
// BEGIN_SignalSlot 0

    //! Connects a Signal to a function.
    template <typename R>
    Connection connect(Signal<>& signal, R(*func)())
    {
        return connect( signal, slot(func) );
    }

    //! Connects a Signal to a member function.
    template <typename R, class BaseT, typename ClassT>
    Connection connect(Signal<>& signal, BaseT& object, R(ClassT::*memFunc)())
    {
        return connect( signal, slot(object, memFunc) );
    }

    //! Connects a Signal to a const member function.
    template <typename R, class BaseT, typename ClassT>
    Connection connect(Signal<>& signal, BaseT& object, R(ClassT::*memFunc)() const)
    {
        return connect( signal, slot(object, memFunc) );
    }

    template <typename R>
    void disconnect(Signal<>& signal, R(*func)())
    {
        signal.disconnect( slot(func) );
    }

    template <typename R, typename BaseT, typename ClassT>
    void disconnect(Signal<>& signal, BaseT & object, R(ClassT::*memFunc)())
    {
        signal.disconnect( slot( object, memFunc ) );
    }

    template <typename R, class BaseT, typename ClassT>
    void disconnect(Signal<>& signal, BaseT& object, R(ClassT::*memFunc)() const)
    {
        signal.disconnect( slot( object, memFunc ) );
    }

    template <typename R>
    void disconnect( Signal<>& sender, Signal<>& receiver )
    {
        sender.disconnect( slot(receiver) );
    }

    /** Connects a Signal to another Signal. */
    inline Connection connect(Signal<>& sender, Signal<>& receiver)
    {
        return connect( sender, slot(receiver) );
    }
// END_SignalSlot 0
