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
        explicit BasicServiceProcedure(std::function<R($tparams)>& cb)
        : ServiceProcedure(),
          _cb(cb),
          _composers(_args, $N)
        {
EOF
for (my $i = 0; $i < $N; ++$i)
{
    my $ii = $i + 1;
print <<EOF;
            _args[$i] = &_a$ii;
EOF
}
print <<EOF;
        }

        ServiceProcedure* clone() const override
        {
            return new BasicServiceProcedure(_cb);
        }

        IComposers* beginCall(const std::string&) override
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

            return &_composers;
        }

        IDecomposer* endCall() override
        {
            _rv = _cb($vars);
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

        std::function<R($tparams)> _cb;
        RV _rv;
EOF
for (my $i = 1; $i <= $N; ++$i)
{
print <<EOF;
        V$i _v$i;
EOF
}
print <<EOF;

        IComposer* _args[$N];
        Composers _composers;
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
        explicit BasicServiceProcedure(const std::function<R($tparams)>& cb)
        : ServiceProcedure(),
          _cb(cb),
          _composers(_args, $nn)
        {
EOF
for (my $i = 0; $i < $nn; ++$i)
{
    my $ii = $i + 1;
print <<EOF;
            _args[$i] = &_a$ii;
EOF
}
print <<EOF;
        }

        ServiceProcedure* clone() const override
        {
            return new BasicServiceProcedure(_cb);
        }

        IComposers* beginCall(const std::string&) override
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

            return &_composers;
        }

        IDecomposer* endCall() override
        {
            _rv = _cb($vars);
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

        std::function<R($tparams)> _cb;
        RV _rv;
EOF
for (my $i = 1; $i <= $nn; ++$i)
{
print <<EOF;
        V$i _v$i;
EOF
}
print <<EOF;

        IComposer* _args[$nn];
        Composers _composers;
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
        explicit BasicServiceProcedure(const std::function<R()>& cb)
        : ServiceProcedure(),
          _cb(cb),
          _composers(nullptr, 0)
        {
        }

        ServiceProcedure* clone() const override
        {
            return new BasicServiceProcedure(_cb);
        }

        IComposers* beginCall(const std::string&) override
        {
            return &_composers;
        }

        IDecomposer* endCall() override
        {
            _rv = _cb();
            _r.begin(_rv);
            return &_r;
        }

    private:
        typedef typename TypeTraits<R>::Value RV;

        std::function<R()> _cb;
        RV _rv;

        Composers _composers;
        Decomposer<RV> _r;
};

//! \@endcond internal
// end generated with `serviceprocedure.pl -n $N`
EOF
