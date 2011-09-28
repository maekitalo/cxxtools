#!/usr/bin/perl -w

=head1 NAME

=head1 SYNOPSIS

=head1 DESCRIPTION

=head1 OPTIONS

=head1 AUTHOR

Tommi ME<auml>kitalo, Tntnet.org

=cut

use strict;
use Getopt::Std;

my %opt;

getopts('n:N:', \%opt);

my $N = $opt{n} || 10;
my $ns = $opt{N} || 'cxxtools';

sub mklist {
  my ($praefix, $n, $postfix) = @_;
  $postfix = '' unless defined($postfix);
  join ', ', map { "$praefix$_$postfix" } (1..$n);
}

########################################################################
## first
##
my $first = 1;

for (my $nn = $N; $nn >= 0; --$nn)
{
  print "// BasicServiceProcedure with $nn arguments\n";
  print "template <typename R";

  my $typeList = $nn ? (", " . mklist('A', $nn)) : '';
  my $varList = mklist('_v', $nn);
  my $n;

  if ($first)
  {
    for (my $n = 1; $n <= $N; ++$n)
    {
      print ",
          typename A$n = cxxtools::Void";
    }

    print <<EOF;
>
class BasicServiceProcedure : public ServiceProcedure
EOF
    $first = 0;
  }
  else
  {
    for (my $n = 1; $n <= $nn; ++$n)
    {
      print ",
          typename A$n";
    }

    my @v = ("${ns}::Void") x ($N - $nn);
    my $voids = join (',
                            ', @v);
    print <<EOF;
>
class BasicServiceProcedure<R$typeList,
                            $voids> : public ServiceProcedure
EOF
  }

    my $CallableT = "Callable<R$typeList>";
    print <<EOF;
{
    public:
        BasicServiceProcedure( const $CallableT& cb )
        : ServiceProcedure()
        , _cb(0)
        {
            _cb = cb.clone();

EOF

  for ($n = 0; $n < $nn; ++$n)
  {
    my $n1 = $n + 1;
    print <<EOF;
            _args[$n] = &_a$n1;
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

        IDeserializer** beginCall()
        {
EOF
  for ($n = 1; $n <= $nn; ++$n)
  {
print <<EOF;
            _a$n.begin(_v$n);
EOF
  }

  print <<EOF;

            return _args;
        }

        ISerializer* endCall()
        {
            _rv = _cb->call($varList);
            _r.begin(_rv);
            return &_r;
        }

    private:
EOF

  for ($n = 1; $n <= $nn; ++$n)
  {
    print <<EOF;
        typedef typename TypeTraits<A$n>::Value V$n;
EOF
}

    print <<EOF;
        typedef typename TypeTraits<R>::Value RV;

        $CallableT* _cb;
        RV _rv;
EOF

  for ($n = 1; $n <= $nn; ++$n)
  {
    print <<EOF;
        V$n _v$n;
EOF
  }

  my $nn1 = $nn + 1;
  print <<EOF;

        IDeserializer* _args[$nn1];
EOF

  for ($n = 1; $n <= $nn; ++$n)
  {
    print <<EOF;
        Deserializer<V$n> _a$n;
EOF
  }

  print <<EOF;
        Serializer<RV> _r;
};


EOF
}

print <<EOF;
class CXXTOOLS_XMLRPC_API Service : public http::Service
{
        friend class XmlRpcResponder;

    public:
        Service()
        { }

        virtual ~Service();

EOF

for (my $n = 0; $n <= $N; ++$n)
{
  my $typenameList = join ('', map { ", typename A$_" } (1..$n));
  my $typeList = mklist('A', $n);
  my $typeList0 = join ('', map { ", A$_" } (1..$n));

print <<EOF;
        template <typename R$typenameList>
        void registerFunction(const std::string& name, R (*fn)($typeList))
        {
            ServiceProcedure* proc = new BasicServiceProcedure<R$typeList0>(cxxtools::callable(fn));
            this->registerProcedure(name, proc);
        }

EOF
}

for (my $n = 0; $n <= $N; ++$n)
{
  my $typenameList = join ('', map { ", typename A$_" } (1..$n));
  my $typeList = mklist('A', $n);
  my $typeList0 = join ('', map { ", A$_" } (1..$n));

print <<EOF;
        template <typename R$typenameList>
        void registerCallable(const std::string& name, const Callable<R$typeList0>& cb)
        {
            ServiceProcedure* proc = new BasicServiceProcedure<R$typeList0>(cb);
            this->registerProcedure(name, proc);
        }

EOF
}

for (my $n = 0; $n <= $N; ++$n)
{
  my $typenameList = join ('', map { ", typename A$_" } (1..$n));
  my $typeList = mklist('A', $n);
  my $typeList0 = join ('', map { ", A$_" } (1..$n));

print <<EOF;
        template <typename R, class C$typenameList>
        void registerMethod(const std::string& name, C& obj, R (C::*method)($typeList) )
        {
            ServiceProcedure* proc = new BasicServiceProcedure<R$typeList0>( callable(obj, method) );
            this->registerProcedure(name, proc);
        }

EOF
}

print <<EOF;
        virtual http::Responder* createResponder(const http::Request&);

        virtual void releaseResponder(http::Responder* resp);

    protected:
        ServiceProcedure* getProcedure(const std::string& name);

        void releaseProcedure(ServiceProcedure* proc);

        void registerProcedure(const std::string& name, ServiceProcedure* proc);

    private:
        typedef std::map<std::string, ServiceProcedure*> ProcedureMap;
        ProcedureMap _procedures;
};

EOF
