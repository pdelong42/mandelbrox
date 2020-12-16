#!/usr/bin/perl

# ./foo.pl 1000 0.425 0.45 0.355 0.380 -2 2 -2 2

use strict;
use warnings;

scalar( @ARGV ) == 9
   or die "nine args required - aborting\n";

my( $steps, $x1_min, $x1_max, $y1_min, $y1_max, $x2_min, $x2_max, $y2_min, $y2_max ) = @ARGV;

sub nth_root { exp( log( shift() ) / $steps ) }

my $x1_mid = ( $x1_max + $x1_min ) / 2;
my $y1_mid = ( $y1_max + $y1_min ) / 2;
my $x2_mid = ( $x2_max + $x2_min ) / 2;
my $y2_mid = ( $y2_max + $y2_min ) / 2;

my $x1_rad = ( $x1_max - $x1_min ) / 2;
my $y1_rad = ( $y1_max - $y1_min ) / 2;
my $x2_rad = ( $x2_max - $x2_min ) / 2;
my $y2_rad = ( $y2_max - $y2_min ) / 2;

my $x_ratio = $x1_rad / $x2_rad;
my $y_ratio = $y1_rad / $y2_rad;

my $x_step = nth_root $x_ratio;
my $y_step = nth_root $y_ratio;

printf "x_ratio = %lf\n", $x_ratio;
printf "y_ratio = %lf\n", $y_ratio;

printf "x_step = %lf\n", $x_step;
printf "y_step = %lf\n", $y_step;
