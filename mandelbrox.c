#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

int c_max = (1<<8) - 1;

int mu = 1;
int nu = 100;

double sixth = M_PI / 3.0;

struct color {
  double red;
  double green;
  double blue;
};

struct params {
  double x_min;
  double x_max;
  double y_min;
  double y_max;
  double bailout;
  int max_iter;
};

void preamble_common( char *format, int width, int height, struct params p ) {

  printf( "%s\n", format );
  printf( "# x_min = %f\n", p.x_min );
  printf( "# x_max = %f\n", p.x_max );
  printf( "# y_min = %f\n", p.y_min );
  printf( "# y_max = %f\n", p.y_max );
  printf( "# bailout = %f\n", p.bailout );
  printf( "# max_iter = %d\n", p.max_iter );
}

void preamble_netpbm( char *format, int width, int height, struct params p ) {

  preamble_common( format, width, height, p );
  printf( "%d %d\n", width, height );
}

void preamble_netpgm( char *format, int width, int height, struct params p ) {

  preamble_netpbm( format, width, height, p );
  printf( "%d\n", c_max );
}

void preamble_netppm( char *format, int width, int height, struct params p ) {

  preamble_netpbm( format, width, height, p );
  printf( "%d\n", c_max );
}

void preamble_netpam( char *format, int width, int height, struct params p ) {

  preamble_common( format, width, height, p );
  printf( "WIDTH %d\nHEIGHT %d\nMAXVAL %d\n", width, height, c_max );
  printf( "TUPLTYPE RGB\nDEPTH 3\nENDHDR\n" );
}

void color_netpbm( int iter, int max_iter ) {
  printf( ( iter < max_iter ) ? "0 " : "1 " );
}

void color_netpgm( int iter, int max_iter ) {
  printf( "%d ", ( c_max * ( max_iter - iter ) ) / max_iter );
}

void color_picker( int iter, int max_iter, struct color *c ) {

  if( iter >= max_iter ) {
    c->red = c->green = c->blue = 0;
    return;
  }

  double tmp;
  double phi   = M_PI / mu;
  double omega = M_PI * nu;
  double ratio = (double)iter / (double)max_iter;
  double theta = phi + omega * ratio;

  tmp = cos( theta - sixth );
  tmp *= tmp;
  c->red = c_max;
  c->red *= tmp;

  tmp = cos( theta );
  tmp *= tmp;
  c->green = c_max;
  c->green *= tmp;

  tmp = cos( theta + sixth );
  tmp *= tmp;
  c->blue = c_max;
  c->blue *= tmp;
}

void color_netppm( int iter, int max_iter ) {

  struct color c;

  color_picker( iter, max_iter, &c );
  printf( "%d %d %d ", (int)c.red, (int)c.green, (int)c.blue );
}

void color_netpam( int iter, int max_iter ) {

  struct color c;

  color_picker( iter, max_iter, &c );
  printf( "%c%c%c", (char)c.red, (char)c.green, (char)c.blue );
}

int main( int argc, char *argv[] ) {

  char format[10] = "P6";
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
    if( 0 == strncmp( "P3", format, fn ) ) {
      print_preamble = &preamble_netppm;
      print_color    =    &color_netppm;
      ++fmt_flag;
    }
    if( 0 == strncmp( "P6", format, fn ) ) {
      print_preamble = &preamble_netppm;
      print_color    =    &color_netpam;
      ++fmt_flag;
    }
    if( 0 == strncmp( "P7", format, fn ) ) {
      print_preamble = &preamble_netpam;
      print_color    =    &color_netpam;
      ++fmt_flag;
    }
  }

  if( fmt_flag == 0 ) { /* P4 and P5 formats are not yet implemented */
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
  }
}
