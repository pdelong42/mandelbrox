#!/usr/bin/perl

=pod

This script is a helper for generating frames for a zoom in/out video.
The input is the boundary values for the first and last frames.  The
output is one line of text to the STDOUT for each extrapolated frame,
where each of those lines is the boundary values for that frame.

This should be piped to the script that is being used to run commands
to generate those frames, whether that be a simple shell script, or
something like OSG (HTC Condor), Slurm, or PBS Pro.

Ultimately, I think I'll probably integrate this feature into the C
codebase of the main program.  But for now, I just wanted a POC in a
langauge I know I could bang it out in relatively quickly.

Example: ./GenZoomParams.pl 100 -2 2 -2 2 0.425 0.45 0.355 0.380 

=cut

use strict;
use warnings;

my $frame = 0;

my $x_scale = 1;
my $y_scale = 1;

scalar( @ARGV ) == 9
   or die "nine args required - aborting\n";

my( $frames, $x1_min, $x1_max, $y1_min, $y1_max, $x2_min, $x2_max, $y2_min, $y2_max ) = @ARGV;

sub nth_root { exp( log( shift() ) / $frames ) }

my $x1_span = $x1_max - $x1_min;
my $y1_span = $y1_max - $y1_min;
my $x2_span = $x2_max - $x2_min;
my $y2_span = $y2_max - $y2_min;

my $x_ratio = $x1_span / $x2_span;
my $y_ratio = $y1_span / $y2_span;

my $x_pop = nth_root $x_ratio;
my $y_pop = nth_root $y_ratio;

my $x_max_delta = $x1_max - $x2_max;
my $y_max_delta = $y1_max - $y2_max;

my $x_min_delta = $x1_min - $x2_min;
my $y_min_delta = $y1_min - $y2_min;

my( $x_min, $x_max, $y_min, $y_max ) = ( $x2_min, $x2_max, $y2_min, $y2_max );

#printf "# x_ratio = %lf; x_pop = %lf\n", $x_ratio, $x_pop;
#printf "# y_ratio = %lf; y_pop = %lf\n", $y_ratio, $y_pop;

sub PrintCommandLine {
    printf "%.20qf %.20qf %.20qf %.20qf\n", @_;
}

PrintCommandLine $x1_min, $x1_max, $y1_min, $y1_max;

while( ++$frame <= $frames ) {

    $x_min_delta /= $x_pop;
    $x_max_delta /= $x_pop;

    $y_min_delta /= $y_pop;
    $y_max_delta /= $y_pop;

    $x_min = $x2_min + $x_min_delta;
    $x_max = $x2_max + $x_max_delta;
    $y_min = $y2_min + $y_min_delta;
    $y_max = $y2_max + $y_max_delta;

    PrintCommandLine $x_min, $x_max, $y_min, $y_max;
}

PrintCommandLine $x2_min, $x2_max, $y2_min, $y2_max;
