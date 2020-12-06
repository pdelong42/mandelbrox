#!/bin/bash

set -xeuo pipefail

CMD="/usr/bin/time ./mandelbrox"

#$CMD -B threads_naive -x 0 -X 2 -y 0 -Y 2 -t 4 -M 1000 -w 1024 -h 1024 > 1.ppm
#$CMD -B threads_naive -x 0 -X 1 -y 0 -Y 1 -t 4 -M 1000 -w 1024 -h 1024 > 1.ppm
#$CMD -B threads_naive -x 0 -X 0.5 -y 0 -Y 0.5 -t 4 -M 1000 -w 1024 -h 1024 > 1.ppm
#$CMD -B threads_simple -x 0 -X 0.5 -y 0 -Y 0.5 -t 4 -M 1000 -w 1024 -h 1024 > 2.ppm
#$CMD -B threads_simple -x 0.25 -X 0.5 -y 0.25 -Y 0.5 -t 4 -M 1000 -w 1024 -h 1024 > 3.ppm
#$CMD -B threads_simple -x 0.375 -X 0.500 -y 0.250 -Y 0.375 -t 4 -M 1000 -w 512 -h 512 > 4.ppm
#$CMD -B threads_simple -x 0.375 -X 0.500 -y 0.300 -Y 0.425 -t 4 -M 1000 -w 512 -h 512 > 4.ppm
#$CMD -B threads_simple -x 0.4 -X 0.5 -y 0.3 -Y 0.4 -t 4 -M 1000 -w 512 -h 512 > 5.ppm
#$CMD -B threads_simple -x 0.40 -X 0.45 -y 0.35 -Y 0.40 -t 4 -M 1000 -w 512 -h 512 > 6.ppm
$CMD -B threads_simple -x 0.425 -X 0.450 -y 0.355 -Y 0.380 -t 4 -M 1000 -w 1024 -h 1024 > 7.ppm
