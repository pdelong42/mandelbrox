#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

int c_max = 256;

int mu = 1;
int nu = 100;

double twopi = 2.0 * M_PI;
double third = 2.0 * M_PI / 3.0;

struct params {
  double x_min;
  double x_max;
  double y_min;
  double y_max;
  double bailout;
  int max_iter;
};

void preamble_netpbm( char *format, int width, int height, struct params p ) {

  printf( "%s\n", format );
  printf( "# x_min = %f\n", p.x_min );
  printf( "# x_max = %f\n", p.x_max );
  printf( "# y_min = %f\n", p.y_min );
  printf( "# y_max = %f\n", p.y_max );
  printf( "# bailout = %f\n", p.bailout );
  printf( "# max_iter = %d\n", p.max_iter );
  printf( "%d %d\n", width, height );
}

void preamble_netpgm( char *format, int width, int height, struct params p ) {

  preamble_netpbm( format, width, height, p );
  printf( "%d\n", c_max - 1 );
}

void preamble_netppm( char *format, int width, int height, struct params p ) {

  preamble_netpbm( format, width, height, p );
  printf( "%d\n", c_max - 1 );
}

void color_netpbm( int iter, int max_iter ) {
  printf( ( iter < max_iter ) ? "0 " : "1 " );
}

void color_netpgm( int iter, int max_iter ) {
  printf( "%d ", ( c_max * ( max_iter - iter ) ) / max_iter );
}

void color_netppm( int iter, int max_iter ) {

  if( iter < max_iter ) {
    double phi   = twopi / mu;
    double omega = twopi * nu;
    double ratio = iter / (double)max_iter;
    double theta = phi + omega * ratio;
    double red   = 0.5 * c_max * ( 1 + sin( theta - third ) );
    double green = 0.5 * c_max * ( 1 + sin( theta         ) );
    double blue  = 0.5 * c_max * ( 1 + sin( theta + third ) );
    printf( "%d %d %d ", (int)red, (int)green, (int)blue );
  } else {
    printf( "0 0 0 " );
  }
}

int main( int argc, char *argv[] ) {

  char format[10] = "P3";
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
    { "Maxiter", required_argument, 0, 'M' },
    { "bailout", required_argument, 0, 'b' },
    {      "mu", required_argument, 0, 'm' },
    {      "nu", required_argument, 0, 'n' },
    {         0,                 0, 0,  0  }
  };

  while( 1 ) {

    int c = getopt_long( argc, argv, "f:M:b:w:h:m:n:", long_options, NULL );

    if( c < 0 ) break;

    switch( c ) {

    case 'f':
      sscanf( optarg, "%9s", format );
      break;

    case 'w':
      sscanf( optarg, "%9d", &width );
      break;

    case 'h':
      sscanf( optarg, "%9d", &height );
      break;

    case 'M':
      sscanf( optarg, "%9d", &p.max_iter );
      break;

    case 'b':
      sscanf( optarg, "%9lf", &p.bailout );
      break;

    case 'm':
      sscanf( optarg, "%9d", &mu );
      break;

    case 'n':
      sscanf( optarg, "%9d", &nu );
      break;

    case '?':
      break;

    default:
      fprintf( stderr, "?? getopt returned character code 0x%X ??\n", c );
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
    if( 0 == strncmp( "P1", format, fn ) ) {
      print_preamble = &preamble_netpbm;
      print_color    =    &color_netpbm;
      ++fmt_flag;
    }
    if( 0 == strncmp( "P2", format, fn ) ) {
      print_preamble = &preamble_netpgm;
      print_color    =    &color_netpgm;
      ++fmt_flag;
    }
    if( 0 == strncmp( "P3", format, fn )
     || 0 == strncmp( "P4", format, fn )
     || 0 == strncmp( "P5", format, fn )
     || 0 == strncmp( "P6", format, fn ) ) {
      print_preamble = &preamble_netppm;
      print_color    =    &color_netppm;
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
