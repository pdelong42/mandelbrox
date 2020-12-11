#!/bin/bash

# This test uses a patch of the complex plane that is contained
# entirely within the Mandelbrot set, such that every point takes the
# same (maximum) amount of time.

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

BASEARGS="-x -0.5 -X 0 -y -0.5 -Y 0 -t 10"
OPTARGS="-M 1000000 -w 100 -h 100"

if test -n "$*" ; then
   ARGS="$BASEARGS $*"
else
   ARGS="$BASEARGS $OPTARGS"
fi

mb a -B plain          $ARGS
mb b -B threads_simple $ARGS
mb c -B threads_naive  $ARGS
