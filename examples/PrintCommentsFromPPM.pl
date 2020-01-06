#!/usr/bin/perl

use strict;
use warnings;

use English qw( -no_match_vars );

++$OUTPUT_AUTOFLUSH;

undef $INPUT_RECORD_SEPARATOR;

my $input = readline STDIN;

foreach( $input =~ m{ ^ ( \# .*? ) $ }xmsg ) {
   print STDERR "$ARG\n";
}

print $input;
