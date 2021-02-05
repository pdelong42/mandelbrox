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
#mb 7 -B plain -x 0.425 -X 0.450 -y 0.355 -Y 0.380 -t 4 -M 1000 -w 1024 -h 1024
#mb 8 -x 0.435 -X 0.450 -y 0.365 -Y 0.380 -M 1000 -w 1024 -h 1024
#mb 9 -x 0.440 -X 0.450 -y 0.360 -Y 0.370 -M 1000 -w 1024 -h 1024
#mb 10 -x 0.443 -X 0.445 -y 0.363 -Y 0.365 -M 1000 -w 1024 -h 1024
#mb 11 -x 0.444 -X 0.445 -y 0.364 -Y 0.365 -M 1000 -w 1024 -h 1024
#mb 12 -x 0.4440 -X 0.4445 -y 0.3640 -Y 0.3645 -M 1000 -w 1024 -h 1024
#mb 13 -x 0.4440 -X 0.4442 -y 0.3642 -Y 0.3644 -M 1000 -w 1024 -h 1024
#mb 14 -x 0.4440 -X 0.4441 -y 0.3643 -Y 0.3644 -M 1000 -w 1024 -h 1024
#mb 15 -x 0.44406 -X 0.44410 -y 0.36431 -Y 0.36435 -M 1000 -w 1024 -h 1024
#mb 16 -x 0.44407 -X 0.44410 -y 0.36432 -Y 0.36435 -M 1000 -w 1024 -h 1024
#mb 17 -x 0.44407 -X 0.44409 -y 0.36432 -Y 0.36434 -M 1000 -w 1024 -h 1024
#mb 18 -x 0.44408 -X 0.44409 -y 0.36433 -Y 0.36434 -M 1000 -w 1024 -h 1024
#mb 19 -x 0.444080 -X 0.444084 -y 0.364331 -Y 0.364335 -M 1000 -w 1024 -h 1024
#mb 20 -x 0.4440800 -X 0.4440837 -y 0.3643316 -Y 0.3643353 -M 1000 -w 1024 -h 1024
#mb 21 -x 0.4440807 -X 0.4440830 -y 0.3643323 -Y 0.3643346 -M 1000 -w 1024 -h 1024

# The next example (22) was the point at which I noticed that I needed to
# switch from using "double" to using "long double".  It was more pronounced in
# the zoom animation, as the lack of precision made it very jittery in the last
# few frames.

mb 22 -x 0.4440807 -X 0.4440830 -y 0.3643323 -Y 0.3643346 -M 10000 -w 1024 -h 1024

# Presumably, I'll need to switch to using GMP sometime later down the
# line.
