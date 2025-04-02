#!/usr/bin/perl -w

=head1 NAME

serviceprocedure.pl

=head1 SYNOPSIS

serviceprocedure.pl -n 8

=head1 DESCRIPTION

Generates template class ServiceProcedure with variable number of arguments.

=head1 OPTIONS

=over
=item
=item
=back

=head1 AUTHOR

Tommi ME<auml>kitalo, Tntnet.org

=cut

use strict;

use strict;
use Getopt::Std;

my %opt;

getopts('n:', \%opt);

my $N = $opt{n} || 10;

########################################################################
## first
##
print <<EOF;
// generated with `serviceprocedure.pl -n $N`
//! \@cond internal
// BasicServiceProcedure with $N arguments
template <typename R,
EOF
for (my $i = 1; $i < $N; ++$i)
{
print <<EOF;
          typename A$i = Void,
EOF
}

my $tparams = join ', ', map { "A$_" } (1..$N);
print <<EOF;
          typename A$N = Void>
class BasicServiceProcedure : public ServiceProcedure
{
    public:
        BasicServiceProcedure( const Callable<R, $tparams>& cb )
        : ServiceProcedure()
        , _cb(0)
        {
            _cb = cb.clone();

EOF
for (my $i = 0; $i < $N; ++$i)
{
    my $ii = $i + 1;
print <<EOF;
            _args[$i] = &_a$ii;
EOF
}
print <<EOF;
            _args[$N] = 0;
        }

        ~BasicServiceProcedure()
        {
            delete _cb;
        }

        ServiceProcedure* clone() const
        {
            return new BasicServiceProcedure(*_cb);
        }

        IComposer** beginCall()
        {
EOF
for (my $i = 1; $i <= $N; ++$i)
{
print <<EOF;
            _a$i.begin(_v$i);
EOF
}
my $vars = join (', ', map { "_v$_" } (1..$N));
print <<EOF;

            return _args;
        }

        IDecomposer* endCall()
        {
            _rv = _cb->call($vars);
            _r.begin(_rv);
            return &_r;
        }

    private:
EOF
for (my $i = 1; $i <= $N; ++$i)
{
print <<EOF;
        typedef typename TypeTraits<A$i>::Value V$i;
EOF
}
print <<EOF;
        typedef typename TypeTraits<R>::Value RV;

        Callable<R, $tparams>* _cb;
        RV _rv;
EOF
for (my $i = 1; $i <= $N; ++$i)
{
print <<EOF;
        V$i _v$i;
EOF
}
my $np = $N + 1;
print <<EOF;

        IComposer* _args[$np];
EOF
for (my $i = 1; $i <= $N; ++$i)
{
print <<EOF;
        Composer<V$i> _a$i;
EOF
}
print <<EOF;
        Decomposer<RV> _r;
};


EOF

########################################################################
## n-th
##
for (my $nn = $N - 1; $nn > 0; --$nn)
{
print <<EOF;
// BasicServiceProcedure with $nn arguments
template <typename R,
EOF
for (my $i = 1; $i < $nn; ++$i)
{
print <<EOF;
          typename A$i,
EOF
}

my $tparams = join ', ', map { "A$_" } (1..$nn);
my $voids = join ', ', map { "Void" } (1..$N-$nn);
print <<EOF;
          typename A$nn>
class BasicServiceProcedure<R, $tparams,
                            $voids> : public ServiceProcedure
{
    public:
        BasicServiceProcedure( const Callable<R, $tparams>& cb )
        : ServiceProcedure()
        , _cb(0)
        {
            _cb = cb.clone();

EOF
for (my $i = 0; $i < $nn; ++$i)
{
    my $ii = $i + 1;
print <<EOF;
            _args[$i] = &_a$ii;
EOF
}
print <<EOF;
            _args[$nn] = 0;
        }

        ~BasicServiceProcedure()
        {
            delete _cb;
        }

        ServiceProcedure* clone() const
        {
            return new BasicServiceProcedure(*_cb);
        }

        IComposer** beginCall()
        {
EOF
for (my $i = 1; $i <= $nn; ++$i)
{
print <<EOF;
            _a$i.begin(_v$i);
EOF
}
my $vars = join (', ', map { "_v$_" } (1..$nn));
print <<EOF;

            return _args;
        }

        IDecomposer* endCall()
        {
            _rv = _cb->call($vars);
            _r.begin(_rv);
            return &_r;
        }

    private:
EOF
for (my $i = 1; $i <= $nn; ++$i)
{
print <<EOF;
        typedef typename TypeTraits<A$i>::Value V$i;
EOF
}
print <<EOF;
        typedef typename TypeTraits<R>::Value RV;

        Callable<R, $tparams>* _cb;
        RV _rv;
EOF
for (my $i = 1; $i <= $nn; ++$i)
{
print <<EOF;
        V$i _v$i;
EOF
}
my $np = $nn + 1;
print <<EOF;

        IComposer* _args[$np];
EOF
for (my $i = 1; $i <= $nn; ++$i)
{
print <<EOF;
        Composer<V$i> _a$i;
EOF
}
print <<EOF;
        Decomposer<RV> _r;
};


EOF
}

########################################################################
## last
##
print <<EOF;
// BasicServiceProcedure with 0 arguments
template <typename R>
class BasicServiceProcedure<R,
                            Void, Void, Void, Void, Void, Void, Void, Void, Void, Void> : public ServiceProcedure
{
    public:
        BasicServiceProcedure( const Callable<R>& cb )
        : ServiceProcedure()
        , _cb(0)
        {
            _cb = cb.clone();

            _args[0] = 0;
        }

        ~BasicServiceProcedure()
        {
            delete _cb;
        }

        ServiceProcedure* clone() const
        {
            return new BasicServiceProcedure(*_cb);
        }

        IComposer** beginCall()
        {

            return _args;
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

        IComposer* _args[1];
        Decomposer<RV> _r;
};

//! \@endcond internal
// end generated with `serviceprocedure.pl -n $N`
EOF
