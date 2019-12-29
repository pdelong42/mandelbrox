This is a Mandelbrot Set sandbox (hence the name: mandelbrox).

Sample usage:

   ./mandelbrox > foo.ppm

And then...

   geeqie foo.ppm

(...or equivalent image viewer that understands PNM formats.)

I'm building-up to doing this using some sort of parallel compute.
But before that, I need to review the basics.

As such, this is pretty rudimentary at the moment.  It only outputs to
PGM, ASCII format (P1).  I'd like to eventually have it output to PPM
binary (P6), and ideally present all PNM formats as runtime options.

I suspect I'll eventually outgrow C's "double" type, and need to look
into the GMP library (GNU Multi-Precision).

I'd also like to try to use pthreads.  Other goals in the parallel
compute options include: MPI, OSG, and GPGPU.

Let's see, other things for the wishlist: command-line parsing of
input parameters, color output for the images, run parameters from an
input file (maybe?).

Instead of specifying a rectangular region explicitly for the complex
plane, specify an eliptical bounding region instead (axes plus
center), and work backwards from that to get the rectangular region.

Consider using the 'm' modifier in format string arguments to scanf().
