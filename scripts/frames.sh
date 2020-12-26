#!/bin/bash

# This is an example of a script that generates frames for a movie.
# It's just an example because too many values are hard-coded.  I
# suppose I could elevate it from the status of an example by
# parametrizing it to give it more general applicablity.  Perhaps
# another time...

set -euo pipefail

FRAME=0
FRAMES=100

SELF=$(realpath $0)
SCRIPTS=$(dirname $SELF)
BASE=$(dirname $SCRIPTS)
DATA=${BASE}/data

COMMON="/usr/bin/time ${BASE}/bin/mandelbrox -B plain -M 1000 -w 1024 -h 1024"
ZOOM="${SCRIPTS}/GenZoomParams.pl $FRAMES -2 2 -2 2 0.425 0.45 0.355 0.380"

while read xmin xmax ymin ymax ; do
    OUTPUT=${DATA}/${FRAME}.ppm
    COMMAND="$COMMON -x $xmin -X $xmax -y $ymin -Y $ymax > $OUTPUT"
    #COMMAND='echo DEBUG: "'${COMMAND}'"'
    sh -x -c "$COMMAND"
    echo saved output to $OUTPUT
    echo ''
    let FRAME=$((1+$FRAME))
done < <($ZOOM)

echo Making a movie...
echo ''

sh -x -c "ffmpeg -i ${DATA}/%d.ppm ${DATA}/foo.mov"
