#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

void preamble_netpbm( char *format, int width, int height, double x_min, double x_max, double y_min, double y_max, int max_iter ) {

  printf( "%s\n", format );
  printf( "%d %d\n", width, height );
  printf( "# x_min = %f\n", x_min );
  printf( "# x_max = %f\n", x_max );
  printf( "# y_min = %f\n", y_min );
  printf( "# y_max = %f\n", y_max );
  printf( "# max_iter = %d\n", max_iter );
}

int main( int argc, char *argv[] ) {

  char format[10] = "P1";
  int width  = 1024;
  int height = 1024;
  int max_iter = 1000;
  double bailout = 4.0;
  double x_min = -2.0, x_max = +2.0;
  double y_min = -2.0, y_max = +2.0;

  static struct option long_options[] = {
    {  "format", required_argument, 0, 'f' },
    {   "width", required_argument, 0, 'w' },
    {  "height", required_argument, 0, 'h' },
    { "bailout", required_argument, 0, 'b' },
    { "maxiter", required_argument, 0, 'm' },
    {         0,                 0, 0,  0  }
  };

  while( 1 ) {

    int c = getopt_long( argc, argv, "f:m:b:w:h:", long_options, NULL );

    if( c < 0 ) break;

    switch( c ) {

    case 'f':
      sscanf( optarg, "%9s", format );
      break;

    case 'm':
      sscanf( optarg, "%9d", &max_iter );
      break;

    case 'w':
      sscanf( optarg, "%9d", &width );
      break;

    case 'h':
      sscanf( optarg, "%9d", &height );
      break;

    case 'b':
      sscanf( optarg, "%9lf", &bailout );
      break;

    case '?':
      break;

    default:
      printf( "?? getopt returned character code 0%o ??\n", c );
    }
  }

  if( optind < argc ) {
    fprintf( stderr, "non-option ARGV-elements: " );
    while( optind < argc ) fprintf( stderr, "%s ", argv[optind++] );
    fprintf( stderr, "\n" );
  }

  if( width <= 0 || height <= 0 ) {
    fprintf( stderr, "ERROR: canvas must not be zero width or height - aborting\n" );
    exit( EXIT_FAILURE );
  }

  int fmt_flag = 0;
  size_t fn = strlen( format );
  void (*preamble)( char *format, int width, int height, double x_min, double x_max, double y_min, double y_max, int max_iter );

  if( fn == 2 ) {
    if( 0 == strncmp( "P1", format, fn )
     || 0 == strncmp( "P2", format, fn )
     || 0 == strncmp( "P3", format, fn )
     || 0 == strncmp( "P4", format, fn )
     || 0 == strncmp( "P5", format, fn )
     || 0 == strncmp( "P6", format, fn ) ) {
      preamble = &preamble_netpbm;
      ++fmt_flag;
    }
  }

  if( fmt_flag == 0 ) {
    fprintf( stderr, "ERROR: specified format \"%s\" is not recognized/supported\n", format );
    exit( EXIT_FAILURE );
  }

  preamble( format, width, height, x_min, x_max, y_min, y_max, max_iter );

  double x_ratio = ( x_max - x_min ) / width;
  double y_ratio = ( y_max - y_min ) / height;

  for( int j = 0; j < height; ++j ) {

    for( int i = 0; i < width; ++i ) {

      int iter = 0;
      double x = 0.0, xx = 0.0;
      double y = 0.0, yy = 0.0;
      double a = x_min + i * x_ratio;
      double b = y_min + j * y_ratio;

      while( ++iter < max_iter ) {

        double xx = x * x;
        double yy = y * y;

        if( xx + yy > bailout ) break;

        double x_new = a + xx - yy;
        double y_new = b + 2 * x * y;

        x = x_new, y = y_new;
      }

      printf( ( iter < max_iter ) ? "0 " : "1 " );
    }
    printf( "\n" );
  }
}
