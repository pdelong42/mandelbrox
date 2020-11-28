#!/bin/bash

set -xeuo pipefail

rm -vf a.ppm b.ppm c.ppm
/usr/bin/time ./mandelbrox -B plain          -t 4 > a.ppm
/usr/bin/time ./mandelbrox -B threads_simple -t 4 > b.ppm
/usr/bin/time ./mandelbrox -B threads_naive  -t 4 > c.ppm
