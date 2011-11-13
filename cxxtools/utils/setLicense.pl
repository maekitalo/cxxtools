#!/usr/bin/perl -w

=head1 NAME

=head1 SYNOPSIS

./setLicense.pl -g lgpl.txt include/cxxtools/*.h

=head1 DESCRIPTION

Replaces the license header with a new license text. Copyright lines are kept
in place.

=head1 OPTIONS

=over 2

=item -g I<filename>

use this file as the copyright text (default: gpl.txt)

=item -f

draw full frame around the copyright text

=item -i I<number>

indent copyright text (default: 1)

=back

=head1 AUTHOR

Tommi ME<auml>kitalo, Tntnet.org

=cut

use strict;
use Getopt::Std;
use File::Copy;

sub eqOnlyWordchar
{
  my ($s1, $s2) = @_;
  $s1 =~ s/\W//g;
  $s2 =~ s/\W//g;
  return $s1 eq $s2;
}

my %opt;
getopts('g:fi:t', \%opt);
my $copyrightFile = $opt{g} || 'gpl.txt';
my $frame = $opt{f};
my $indent = defined($opt{i}) ? $opt{i} : 1;
my $indentspace = ' ' x $indent;
my $test = $opt{t};

# read copyright text
my $copyright;
open C, "<$copyrightFile" or die "cannot open copyright file $copyrightFile: $!";
{
  local $/ = undef;
  $copyright = <C>;
}
close C;

foreach my $fname (@ARGV)
{
  print "process $fname ... ";

  # read content of file
  my $content;
  open IN, "<$fname" or die "cannot open file $fname: $!";
  {
    local $/ = undef;
    $content = <IN>;
    $content =~ s/\r//g;
  }
  close IN;
  my $oldcontent = $content;

  # read copyright lines
  my ($header, $body) = ($content =~ m{\s*(/\*.*?\*/\n?)?(.*)}s);
  my (@copyright, $copyrighttext);
  if ($header)
  {
    while ($header =~ s/(Copyright.*[^*\s])\s*($|\s*\*\s*)//mi)
    {
      my $l = $1;
      $l =~ s/ä/ae/g;
      $l =~ s/ü/ue/g;
      $l =~ s/ö/öe/g;
      $l =~ s/Ä/Ae/g;
      $l =~ s/Ö/Oe/g;
      $l =~ s/Ü/Ue/g;
      $l =~ s/ß/ss/g;
      push @copyright, $l;
    }
    if (@copyright)
    {
      $copyrighttext = join ("\n", @copyright) . "\n\n" . $copyright;
    }
    else
    {
      $copyrighttext = $copyright;
    }
  }
  else
  {
    $copyrighttext = $copyright;
  }

  # add frame
  my @copyrighttext = split(/\n/, $copyrighttext);
  if ($frame)
  {
    my $maxlen = 0;
    foreach (@copyrighttext)
    {
      $maxlen = length($_) if length($_) > $maxlen;
    }
    $copyrighttext = '/***' . ('*' x ($maxlen + $indent)) . "\n"
                   .  join("\n",
                        map {
                          sprintf(" *%s%-*s *", $indentspace, $maxlen, $_);
                        } @copyrighttext)
                   . "\n ***" . ('*' x ($maxlen + $indent)) . "/";
  }
  else
  {
    $copyrighttext = "/*\n"
                   . join("\n",
                        map { " *$indentspace$_"; } @copyrighttext)
                   . "\n */";
  }

  $content = "$copyrighttext\n$body";

  if (!eqOnlyWordchar($content, $oldcontent))
  {
    unless ($test)
    {
      move($fname, "$fname.bak") or die "cannot rename file $fname: $!";
      open OUT, ">$fname" or die "cannot open file $fname: $!";
      print OUT $content;
      close OUT;
    }
    print "ok\n";
  }
  else
  {
    print "not modified\n";
  }
}
