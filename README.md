This is a Mandelbrot Set sandbox (hence the name: mandelbrox).

A simple example:
```
   ./mandelbrox > foo.ppm
```
...and then:
```
   geeqie foo.ppm
```
(...or equivalent image viewer that understands PNM formats.)

A more complicated example:
```
   /usr/bin/time ./mandelbrox -x -1 -X 0 -y 0 -Y +1 -w 4096 -h 4096 | pnmtopng > foo.png && geeqie foo.png &
```
I'm building-up to doing this using some sort of parallel compute.
But before that, I need to review the basics.

As such, this is pretty rudimentary at the moment.  It only outputs to
NetPBM formats for now, because they're super easy to implement (no
libraries / API calls necessary).

As I zoom in deeper and deeper, I suspect I'll eventually outgrow C's
"double" type, and need to look into the GMP library (GNU
Multi-Precision).

I'd also like to try to use pthreads.  Other goals in the parallel
compute options include: MPI, OSG, and GPGPU.

Other things on the to-do list...

Write an adjunct helper program to read an existing image, and do one
of the following:

 - pick a quadrant to zoom into;

 - pick a center point, and zoom in or out around that point, based on
   a scale factor;

...and output the parameters to pass to the main rendering program.

Consider using the 'm' modifier in format string arguments to scanf().

Look into ways of doing periodicity checking as an optimization.

Figure-out a way to increase max_iter dynamically.

Explore an approach wherein the iteration loop is the outer loop
rather than the inner loop (compared with the two loops that scan the
two-dimensional canvas).  At each iteration, the number of new points
found to be outside of the Mandelbrot set is counted, and the loop
exits when that number reaches zero (or when it "effectively" reaches
zero, as in when it drops below a chosen threshold).  This would
essentially make max_iter unnecessary.

Make a less naive color-picking algorithm.  Perhaps something
logarithmic, or which approaches a chosen color as an asymptote.
