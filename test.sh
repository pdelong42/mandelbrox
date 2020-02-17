#!/bin/bash

set -xeuo pipefail

/usr/bin/time ./mandelbrox -B plain         -t 4 > a.ppm
/usr/bin/time ./mandelbrox -B threads_naive -t 4 > b.ppm
