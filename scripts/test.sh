#!/bin/bash

# This test uses a patch of the complex plane that is contained
# entirely within the Mandelbrot set, such that every point takes the
# same amount of time.

set -xeuo pipefail

CMD="/usr/bin/time ./mandelbrox"
BASEARGS="-x -0.5 -X 0 -y -0.5 -Y 0 -t 10"
OPTARGS="-M 1000000 -w 100 -h 100"

if test -n "$*" ; then
   ARGS="$BASEARGS $*"
else
   ARGS="$BASEARGS $OPTARGS"
fi

rm -vf a.ppm b.ppm c.ppm

$CMD -B plain          $ARGS > a.ppm
$CMD -B threads_simple $ARGS > b.ppm
$CMD -B threads_naive  $ARGS > c.ppm
