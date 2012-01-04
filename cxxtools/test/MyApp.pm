package MyApp;

use base qw(JSON::RPC::Procedure); # Perl 5.6 or more than

sub echo : Public {    # new version style. called by clients
    # first argument is JSON::RPC::Server object.
    return $_[1];
}

sub add : Public(a:num, b:num) { # sets value into object member a, b.
    my ($s, $obj) = @_;
    # return a scalar value or a hashref or an arryaref.
    return $obj->{a} + $obj->{b};
}

1;
