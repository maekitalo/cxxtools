#!/usr/bin/perl -w

=head1 NAME

=head1 SYNOPSIS

=head1 DESCRIPTION

=head1 OPTIONS

=head1 AUTHOR

Tommi ME<auml>kitalo, Tntnet.org

=cut

use strict;
use JSON::RPC::Client;

my $client = new JSON::RPC::Client;

my $uri = 'http://localhost:7002/jsonrpc';
my $obj = {
   method  => 'add', # or 'MyApp.sum'
   params  => [10, 20],
};

my $res = $client->call( $uri, $obj );

if($res){
   if ($res->is_error) {
       print "Error : ", $res->error_message;
   }
   else {
       print $res->result;
   }
}

