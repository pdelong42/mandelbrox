#!/usr/bin/perl

=pod

This script is a helper for generating frames for a zoom in/out video.
Its output is one line of text to the STDOUT for each frame, where
each of those lines is the full command (with args) for generating
that frame.

In principle, one could pipe the output straight into bash and be off
to the races.  But in reality, that's a bit of a clumsy way to use
this, and the process isn't terribly good with visual feedback.
Instead, you'll probably want to divvy up the output into batches, to
submit to a work queue system of some sort (e.g., Slurm, PBS Pro,
Condor, OSG, etc.)

Ultimately, I think I'll probably integrate this feature into the C
codebase of the main program.  But for now, I just wanted a POC in a
langauge I know I could bang it out in relatively quickly.

Example: ./frames.pl 100 -2 2 -2 2 0.425 0.45 0.355 0.380 

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

print <<'EOF';
#!/bin/bash

set -euo pipefail

SELF=$(realpath $0)
SCRIPTS=$(dirname $SELF)
BASE=$(dirname $SCRIPTS)

function mb {
    OUTPUT=${BASE}/data/${1}.ppm
    shift
    sh -x -c "/usr/bin/time ${BASE}/bin/mandelbrox $* > $OUTPUT"
    echo ''
    echo saved output to $OUTPUT
}

EOF

printf "# x_ratio = %lf; x_pop = %lf\n", $x_ratio, $x_pop;
printf "# y_ratio = %lf; y_pop = %lf\n", $y_ratio, $y_pop;

sub PrintCommandLine {
    printf "mb %07d -B plain -x %lf -X %lf -y %lf -Y %lf -M 1000 -w 1024 -h 1024\n", @_;
}

PrintCommandLine $frame, $x1_min, $x1_max, $y1_min, $y1_max;

while( ++$frame <= $frames ) {

    $x_min_delta /= $x_pop;
    $x_max_delta /= $x_pop;

    $y_min_delta /= $y_pop;
    $y_max_delta /= $y_pop;

    $x_min = $x2_min + $x_min_delta;
    $x_max = $x2_max + $x_max_delta;
    $y_min = $y2_min + $y_min_delta;
    $y_max = $y2_max + $y_max_delta;

    PrintCommandLine $frame, $x_min, $x_max, $y_min, $y_max;
}

PrintCommandLine $frame, $x2_min, $x2_max, $y2_min, $y2_max;
