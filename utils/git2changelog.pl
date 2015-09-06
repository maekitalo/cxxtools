#!/usr/bin/perl -w

=head1 NAME

=head1 SYNOPSIS

=head1 DESCRIPTION

=head1 OPTIONS

=head1 AUTHOR

Tommi ME<auml>kitalo, Tntnet.org

=cut

use strict;

my $author;
my $date = '';
my %month = (
    Jan => 1,
    Feb => 2,
    Mar => 3,
    Apr => 4,
    May => 5,
    Jun => 6,
    Jul => 7,
    Aug => 8,
    Sep => 9,
    Oct => 10,
    Nov => 11,
    Dec => 12
);

my $content = '';

my $firstline;

while (<>)
{
    next if /^commit/ || /^\s*$/;

    if (/^Author:\s*(.+)/)
    {
        $author = $1;
    }
    elsif (my ($month, $day, $year) = /^Date:\s+\w+\s+(\w+)\s(\d+)\s\S+\s*(\d+)/)
    {
        $month = $month{$month} or die "invalid month $month";
        my $newdate = sprintf('%04d-%02d-%02d', $year, $month, $day);
        if ($newdate ne $date)
        {
            $date = $newdate;
            print "\n$date $author\n\n";
        }

        $firstline = 1;
    }
    else
    {
        s/^    //;
        if ($firstline)
        {
            print " - ";
            undef($firstline);
        }
        else
        {
            print "   ";
        }

        print;
    }
}
