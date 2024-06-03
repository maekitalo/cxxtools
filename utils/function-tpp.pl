#!/usr/bin/perl -w

=head1 NAME

function-tpp.pl

=head1 SYNOPSIS

function-tpp.pl -n 10

=head1 DESCRIPTION

Generates template class Function with variable number of arguments.

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

sub classdef
{
    my ($n) = @_;

    print "template <typename R";

    print ", " unless $n == 0;

    if ($n == $N)
    {
        for (my $nn = 1; $nn <= $N; ++$nn)
        {
            print ", " unless $nn == 1;
            print "class A$nn";
        }

        print ">\nclass Function : public Callable<R, ";

        print argumentTypes($N);
    }
    else
    {
        print argumentClasses($n);
        print ">\nclass Function<R";
        print ", " unless $n == 0;
        print argumentTypes($n);

        for (my $nn = $n; $nn < $N; ++$nn)
        {
            print ", Void";
        }

        print "> : public Callable<R";

        for (my $nn = 1; $nn <= $n; ++$nn)
        {
            print ", A$nn";
        }

        for (my $nn = $n; $nn < $N; ++$nn)
        {
            print ", Void";
        }
    }

    print ">\n";
}

my $argumentClassesDef = argumentClassesDef($N);
print <<EOF;
/**
 The Function class wraps std functions in the form of a Callable, for use
with the signals/slots framework.
*/

template <typename R, $argumentClassesDef>
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


EOF

for (my $n = $N; $n >= 0; --$n)
{
    my $argumentTypes = argumentTypes($n);
    my $commaArgumentTypes = $n == 0 ? $argumentTypes : ", $argumentTypes";
    my $argumentVars = argumentVars($n);
    my $argumentClasses = argumentClasses($n);
    my $commaArgumentClasses = $n == 0 ? $argumentClasses : ", $argumentClasses";
    my $arguments = arguments($n);

    print <<EOF;
// BEGIN_Function $n
EOF
    classdef($n);
print <<EOF;
{
    public:
        /** The function signature wrapped by this class. */
        typedef std::function<R($argumentTypes)> FuncT;

        /** Wraps func. */
        Function(FuncT func)
        : _function(func) { }

        /** Deeply copies f. */
        Function(const Function& f) = default;

        /** Call the wrapped function, passing it the arguments as-is and returning
        its return value. */
        R operator()($arguments) const
        { return _function($argumentVars); }

        /** Creates a clone of this object and returns it. The caller owns the returned object. */
        Function<R$commaArgumentTypes>* clone() const
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
template <typename R$commaArgumentClasses>
Function<R$commaArgumentTypes> callable(R (*func)($arguments))
{ return Function<R$commaArgumentTypes>(func); }

/** Creates and returns a FunctionSlot object for the given free/static function. */
template <typename R$commaArgumentClasses>
FunctionSlot<R$commaArgumentTypes> slot( R (*func)($arguments) )
{ return FunctionSlot<R$commaArgumentTypes>( callable(func) ); }

/** Creates and returns a FunctionSlot object for the given std::function. */
template <typename R$commaArgumentClasses>
FunctionSlot<R$commaArgumentTypes> slot( const std::function<R($argumentTypes)>& func )
{ return FunctionSlot<R$commaArgumentTypes>( func ); }

// END_Function $n
EOF
}
