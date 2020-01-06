#!/bin/bash

set -euo pipefail

TMPFILE=$(mktemp)

function cleanup {
   echo -n "Cleaning-up: " 1>&2
   rm -vf $TMPFILE         1>&2
}

trap cleanup SIGTERM SIGINT SIGQUIT SIGABRT SIGKILL EXIT

tee >(awk '/^#.*=/ {gsub("^# ","");gsub(" = "," ");print}' > ${TMPFILE}) | pnmtopng -text $TMPFILE
