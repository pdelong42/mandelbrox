#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

struct params {
  double x_min;
  double x_max;
  double y_min;
  double y_max;
  int max_iter;
  double bailout;
};

void preamble_netpbm( char *format, int width, int height, struct params p ) {

  printf( "%s\n", format );
  printf( "%d %d\n", width, height );
  printf( "# x_min = %f\n", p.x_min );
  printf( "# x_max = %f\n", p.x_max );
  printf( "# y_min = %f\n", p.y_min );
  printf( "# y_max = %f\n", p.y_max );
  printf( "# bailout = %f\n", p.bailout );
  printf( "# max_iter = %d\n", p.max_iter );
}

void color_netpbm( int iter, int max_iter ) {
  printf( ( iter < max_iter ) ? "0 " : "1 " );
}

int main( int argc, char *argv[] ) {

  char format[10] = "P1";
  int width  = 1024;
  int height = 1024;
  struct params p;

  p.max_iter = 1000;
  p.bailout = 4.0;
  p.x_min = -2.0, p.x_max = +2.0;
  p.y_min = -2.0, p.y_max = +2.0;

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
      sscanf( optarg, "%9d", &p.max_iter );
      break;

    case 'w':
      sscanf( optarg, "%9d", &width );
      break;

    case 'h':
      sscanf( optarg, "%9d", &height );
      break;

    case 'b':
      sscanf( optarg, "%9lf", &p.bailout );
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
  void (*print_preamble)( char *format, int width, int height, struct params p );
  void (*print_color)( int iter, int max_iter );

  if( fn == 2 ) {
    if( 0 == strncmp( "P1", format, fn )
     || 0 == strncmp( "P2", format, fn )
     || 0 == strncmp( "P3", format, fn )
     || 0 == strncmp( "P4", format, fn )
     || 0 == strncmp( "P5", format, fn )
     || 0 == strncmp( "P6", format, fn ) ) {
      print_preamble = &preamble_netpbm;
      print_color    =    &color_netpbm;
      ++fmt_flag;
    }
  }

  if( fmt_flag == 0 ) {
    fprintf( stderr, "ERROR: specified format \"%s\" is not recognized/supported\n", format );
    exit( EXIT_FAILURE );
  }

  print_preamble( format, width, height, p );

  double x_ratio = ( p.x_max - p.x_min ) / width;
  double y_ratio = ( p.y_max - p.y_min ) / height;
  double bailout = p.bailout;
  int max_iter = p.max_iter;

  for( int j = 0; j < height; ++j ) {

    for( int i = 0; i < width; ++i ) {

      int iter = 0;
      double x = 0.0, xx = 0.0;
      double y = 0.0, yy = 0.0;
      double a = p.x_min + i * x_ratio;
      double b = p.y_min + j * y_ratio;

      while( ++iter < max_iter ) {

        double xx = x * x;
        double yy = y * y;

        if( xx + yy > bailout ) break;

        double x_new = a + xx - yy;
        double y_new = b + 2 * x * y;

        x = x_new, y = y_new;
      }

      print_color( iter, max_iter );
    }
    printf( "\n" );
  }
}
