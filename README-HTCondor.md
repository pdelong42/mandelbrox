If you're running this on an HTCondor cluster, first run "make" to build the
executables.  Then just type:

   ./scripts/condor_wrapper.sh

...from the same directory as this readme; or cd to "scripts" and run:

   ./condor_wrapper.sh

...from there.

The script makes a reasonable attempt to get its bearings and find the files it
needs, regardless of where it's invoked from.

It doesn't yet invoke ffmpeg to stitch all the frames together into a movie
file, but that's currently in the works.  Presently, that's done by a separate
standalone submit file, which assumes this first job has already run
successfully, and which also assumes the ffmpeg (statically linked) executable
has already been downloaded and placed into the "bin" directory.

(Apologies, the makefile has a bug, but not a fatal one - it builds the targets
every time, regardless of whether they need it.)
