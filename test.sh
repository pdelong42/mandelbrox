#!/bin/bash

# This test uses a patch of the complex plane that is contained
# entirely within the Mandelbrot set, such that every point takes the
# same amount of time.

set -xeuo pipefail

OPTARGS="-t 4 -M 1000000 -w 100 -h 100"
BASEARGS="-x -0.5 -X 0 -y -0.5 -Y 0"

if test -n "$*" ; then
   ARGS="$BASEARGS $*"
else
   ARGS="$BASEARGS $OPTARGS"
fi

rm -vf a.ppm b.ppm c.ppm

/usr/bin/time ./mandelbrox -B plain          $ARGS > a.ppm
/usr/bin/time ./mandelbrox -B threads_simple $ARGS > b.ppm
/usr/bin/time ./mandelbrox -B threads_naive  $ARGS > c.ppm
