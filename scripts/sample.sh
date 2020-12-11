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

#mb 1 -B threads_naive -x 0 -X 2 -y 0 -Y 2 -t 4 -M 1000 -w 1024 -h 1024
#mb 1 -B threads_naive -x 0 -X 1 -y 0 -Y 1 -t 4 -M 1000 -w 1024 -h 1024
#mb 1 -B threads_naive -x 0 -X 0.5 -y 0 -Y 0.5 -t 4 -M 1000 -w 1024 -h 1024
#mb 2 -B threads_simple -x 0 -X 0.5 -y 0 -Y 0.5 -t 4 -M 1000 -w 1024 -h 1024
#mb 3 -B threads_simple -x 0.25 -X 0.5 -y 0.25 -Y 0.5 -t 4 -M 1000 -w 1024 -h 1024
#mb 3 -B threads_simple -x 0.375 -X 0.500 -y 0.250 -Y 0.375 -t 4 -M 1000 -w 512 -h 512
#mb 4 -B threads_simple -x 0.375 -X 0.500 -y 0.300 -Y 0.425 -t 4 -M 1000 -w 512 -h 512
#mb 5 -B threads_simple -x 0.4 -X 0.5 -y 0.3 -Y 0.4 -t 4 -M 1000 -w 512 -h 512
#mb 6 -B threads_simple -x 0.40 -X 0.45 -y 0.35 -Y 0.40 -t 4 -M 1000 -w 512 -h 512
#mb 7 -B threads_simple -x 0.425 -X 0.450 -y 0.355 -Y 0.380 -t 4 -M 1000 -w 1024 -h 1024
#mb 7 -B plain -x 0.425 -X 0.450 -y 0.355 -Y 0.380 -t 4 -M 1000 -w 1024 -h 1024
mb 7 -B plain -x 0.425 -X 0.450 -y 0.355 -Y 0.380 -t 4 -M 1000 -w 1024 -h 1024
