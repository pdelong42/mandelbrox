#!/bin/bash

set -euo pipefail

AWKFILE=$(mktemp)
TMPFILE=$(mktemp)

echo '/^#.*=/ {gsub("^# ","");gsub(" = "," ");print}' > $AWKFILE

function cleanup {
   echo "Cleaning-up..."    1>&2
   rm -vf $AWKFILE $TMPFILE 1>&2
}

trap cleanup SIGTERM SIGINT SIGQUIT SIGABRT SIGKILL EXIT

tee >(awk -f $AWKFILE > $TMPFILE) | pnmtopng -text $TMPFILE

# Example Usage:
#
# ppmtopng.sh < foo.ppm > foo.png
