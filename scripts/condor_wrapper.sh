#!/bin/sh

# It should be possible to run this from any directory, and it will find the
# files it needs.

set -euo pipefail

SCRIPTS=$(dirname $(realpath $0))
BASEDIR=$(dirname $SCRIPTS)
DATADIR=${BASEDIR}/data
BINDIR=${BASEDIR}/bin

mkdir -p $DATADIR
TEMPSUB=${DATADIR}/$(date +%F_%T).sub
# I went with a resolution of one second, because if you're doing this more
# than once a second, then you're probably doing something wrong.

# this is far less ugly than backslash-escaping the condor parameters, or doing
# multiple heredocs with different quoting
ARGS='-x $(xmin) -X $(xmax) -y $(ymin) -Y $(ymax) -M 10000 -w 1024 -h 1024'
STUB=${DATADIR}/'$(Cluster).$(Process)'

cat <<EOF >> $TEMPSUB
universe = vanilla 

request_cpus = 1 
request_disk = 100 MB 

should_transfer_files = YES 
when_to_transfer_output = ON_EXIT 

Notification = never 

+ProjectName = "AMNH"

#initialdir = $BASEDIR

executable = ${BINDIR}/mandelbrox-static

output = ${STUB}.ppm
error  = ${STUB}.err
log    = ${STUB}.log

arguments = "${ARGS}"

queue xmin,xmax,ymin,ymax from ${SCRIPTS}/GenZoomParams.pl 200 -2 2 -2 2 0.4440807 0.4440830 0.3643323 0.3643346 |

# DEBUG info:
# PWD = $PWD
EOF

echo The submit file for this run can be found here: $TEMPSUB

# uncomment to do dry-run only
#sed s/^/DEBUG:\ / $TEMPSUB
#env LANG=C rm -vf $TEMPSUB
#exit

condor_submit $TEMPSUB

if command -v condor_watch_q > /dev/null ; then
   condor_watch_q -exit all,done,0 -exit any,held,1
else
   echo "Sorry, it looks like condor_watch_q isn't installed, so you'll just have to make do with condor_q..."
fi
