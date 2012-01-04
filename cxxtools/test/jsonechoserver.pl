#!/usr/bin/perl -w

=head1 NAME

=head1 SYNOPSIS

=head1 DESCRIPTION

=head1 OPTIONS

=head1 AUTHOR

Tommi ME<auml>kitalo, Tntnet.org

=cut

use strict;
use JSON::RPC::Server::Daemon;

JSON::RPC::Server::Daemon->new(LocalPort => 8080)
                         ->dispatch({'/jsonrpc' => 'MyApp'})
                         ->handle();


