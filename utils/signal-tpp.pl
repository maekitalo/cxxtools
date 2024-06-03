#!/usr/bin/perl -w

=head1 NAME

signal-tpp.pl

=head1 SYNOPSIS

signal-tpp.pl -n 10

=head1 DESCRIPTION

Generates template class Signal with variable number of arguments.

=head1 OPTIONS

=over
=item

-n <number>
    maximum number of arguments (default 10)

=back

=head1 AUTHOR

Tommi ME<auml>kitalo, Tntnet.org

=cut

use strict;
use Getopt::Std;

my %opt;

getopts('n:', \%opt);

my $N = $opt{n} || 10;

my $classname = 'Function';

sub argumentTypes
{
    my ($n) = @_;

    my $result = '';
    for (my $nn = 1; $nn <= $n; ++$nn)
    {
        $result .= ", " unless $nn == 1;
        $result .= "A$nn";
    }

    return $result;
}

sub arguments
{
    my ($n) = @_;

    my $result = '';
    for (my $nn = 1; $nn <= $n; ++$nn)
    {
        $result .= ", " unless $nn == 1;
        $result .= "A$nn a$nn";
    }

    return $result;
}

sub argumentVars
{
    my ($n) = @_;

    my $result = '';
    for (my $nn = 1; $nn <= $n; ++$nn)
    {
        $result .= ", " unless $nn == 1;
        $result .= "a$nn";
    }

    return $result;
}

sub argumentClasses
{
    my ($n) = @_;

    my $result = '';
    for (my $nn = 1; $nn <= $n; ++$nn)
    {
        $result .= ", " unless $nn == 1;
        $result .= "class A$nn";
    }

    return $result;
}

sub argumentClassesDef
{
    my ($n) = @_;

    my $result = '';
    for (my $nn = 1; $nn <= $n; ++$nn)
    {
        $result .= ", " unless $nn == 1;
        $result .= "class A$nn = Void";
    }

    return $result;
}

my $argumentClassesDef = argumentClassesDef($N);
print <<EOF;
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
        return signal.connect(slot(func));
    }

    //! Connects a Signal to a std::function.
    template <typename R, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
    Connection connect(Signal<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>& signal, std::function<R(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10)>& func)
    {
        return signal.connect(slot(func));
    }

    //! Connects a Signal to a member function.
    template <typename R, class BaseT, typename ClassT, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
    Connection connect(Signal<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>& signal, BaseT& object, R(ClassT::*memFunc)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10))
    {
        return signal.connect(slot(object, memFunc));
    }

    //! Connects a Signal to a const member function.
    template <typename R, class BaseT, typename ClassT, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
    Connection connect(Signal<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>& signal, BaseT& object, R(ClassT::*memFunc)(A1, A2, A3, A4, A5, A6, A7, A8, A9, A10) const)
    {
        return signal.connect(slot(object, memFunc));
    }

    /// Connects a Signal to another Signal
/**
    template <class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9, class A10>
    Connection connect(Signal<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>& sender, Signal<A1, A2, A3, A4, A5, A6, A7, A8, A9, A10>& receiver)
    {
        return sender.connect(slot(receiver));
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
        return sender.connect(slot(receiver));
    }
// END_SignalSlot 10
EOF

for (my $n = $N - 1; $n > 0; --$n)
{
    my $argumentTypes = argumentTypes($n);
    my $commaArgumentTypes = $n == 0 ? $argumentTypes : ", $argumentTypes";
    my $argumentVars = argumentVars($n);
    my $argumentClasses = argumentClasses($n);
    my $commaArgumentClasses = $n == 0 ? $argumentClasses : ", $argumentClasses";
    my $arguments = arguments($n);
    my $voids = ", Void" x ($N - $n);

    print <<EOF;
// BEGIN_Signal $n
    // specialization
    template <$argumentClasses>
    class Signal<$argumentTypes$voids> : public SignalBase {
        public:
            typedef Invokable<$argumentTypes$voids> InvokableT;

        public:
            /**
            Connects slot to this signal, such that firing this signal
            will invoke slot.
            */
            template <typename R>
            Connection connect(const BasicSlot<R, $argumentTypes$voids>& slot)
            {
                return Connection(*this, slot.clone() );
            }

            /** The converse of connect(). */
            template <typename R>
            void disconnect(const BasicSlot<R, $argumentTypes$voids>& slot)
            {
                this->disconnectSlot(slot);
            }

            /**
            Invokes all slots connected to this signal, in an undefined
            order. Their return values are ignored. Calling of connected slots will
            be interrupted if a slot deletes this Signal object or throws an exception.
            */
            inline void send($arguments) const
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
                    invokable->invoke($argumentVars);

                    // if this signal gets deleted by the slot, the Sentry
                    // will be detached. In this case we bail out immediately
                    if( !sentry )
                        return;
                }
            }

            /** Same as send(...). */
            inline void operator()($arguments) const
            { this->send($argumentVars); }
    };

// END_Signal $n
// BEGIN_SignalSlot $n

    //! Connects a Signal to a function.
    template <typename F, $argumentClasses>
    Connection connect(Signal<$argumentTypes>& signal, F&& func)
    {
        return signal.connect(FunctionSlot<void, $argumentTypes>(std::function<void($argumentTypes)>(func)));
    }

    //! Connects a Signal to a member function.
    template <typename R, class BaseT, typename ClassT, $argumentClasses>
    Connection connect(Signal<$argumentTypes>& signal, BaseT& object, R(ClassT::*memFunc)($argumentTypes))
    {
        return signal.connect(slot(object, memFunc));
    }

    //! Connects a Signal to a const member function.
    template <typename R, class BaseT, typename ClassT, $argumentClasses>
    Connection connect(Signal<$argumentTypes>& signal, BaseT& object, R(ClassT::*memFunc)($argumentTypes) const)
    {
        return signal.connect(slot(object, memFunc));
    }

    /// Connects a Signal to another Signal
/**
    template <$argumentClasses>
    Connection connect(Signal<$argumentTypes>& sender, Signal<$argumentTypes>& receiver)
    {
        return sender.connect(slot(receiver));
    }
*/

    template <typename R, $argumentClasses>
    void disconnect(Signal<$argumentTypes>& signal, R(*func)($argumentTypes))
    {
        signal.disconnect( slot(func) );
    }

    template <typename R, typename BaseT, typename ClassT, $argumentClasses>
    void disconnect(Signal<$argumentTypes>& signal, BaseT & object, R(ClassT::*memFunc)($argumentTypes))
    {
        signal.disconnect( slot( object, memFunc ) );
    }

    template <typename R, class BaseT, typename ClassT, $argumentClasses>
    void disconnect(Signal<$argumentTypes>& signal, BaseT& object, R(ClassT::*memFunc)($argumentTypes) const)
    {
        signal.disconnect( slot( object, memFunc ) );
    }

    template <typename R, $argumentClasses>
    void disconnect( Signal<$argumentTypes>& sender, Signal<$argumentTypes>& receiver )
    {
        sender.disconnect( slot(receiver) );
    }

    /** Connects a Signal to another Signal. */
    template <$argumentClasses>
    Connection connect(Signal<$argumentTypes>& sender, Signal<$argumentTypes>& receiver)
    {
        return sender.connect(slot(receiver));
    }
// END_SignalSlot $n
EOF
}

print <<EOF
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
    template <typename F>
    Connection connect(Signal<>& signal, F&& func)
    {
        return signal.connect(FunctionSlot<void>(std::function<void()>(func)));
    }

    template <typename R, class BaseT, typename ClassT>
    Connection connect(Signal<>& signal, BaseT& object, R(ClassT::*memFunc)())
    {
        return signal.connect(slot(object, memFunc));
    }

    //! Connects a Signal to a const member function.
    template <typename R, class BaseT, typename ClassT>
    Connection connect(Signal<>& signal, BaseT& object, R(ClassT::*memFunc)() const)
    {
        return signal.connect(slot(object, memFunc));
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
        return sender.connect(slot(receiver));
    }
// END_SignalSlot 0
EOF
