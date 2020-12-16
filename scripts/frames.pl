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

Example: ./frames.pl 1000 0.425 0.45 0.355 0.380 -2 2 -2 2

=cut

use strict;
use warnings;

scalar( @ARGV ) == 9
   or die "nine args required - aborting\n";

my( $frames, $x1_min, $x1_max, $y1_min, $y1_max, $x2_min, $x2_max, $y2_min, $y2_max ) = @ARGV;

sub nth_root { exp( log( shift() ) / $frames ) }

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

# Obviously, this isn't finished yet.  The only output I'm producing
# so-far is just for intermediate sanity-checking purposes.
