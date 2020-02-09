#include <math.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <pthread.h>

// I lifted this directly from the PTHREAD_CREATE(3) manpage
#define handle_error_en(en, msg) \
   do { errno = en; perror( msg ); exit( EXIT_FAILURE ); } while (0)

int c_max = (1<<8) - 1;

int mu = 2;
int nu = 50;

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

void preamble_common( char *format, int width, int height, struct params *p ) {

  printf( "%s\n", format );
  printf( "# x_min = %f\n",    p->x_min );
  printf( "# x_max = %f\n",    p->x_max );
  printf( "# y_min = %f\n",    p->y_min );
  printf( "# y_max = %f\n",    p->y_max );
  printf( "# bailout = %f\n",  p->bailout );
  printf( "# max_iter = %d\n", p->max_iter );
}

void preamble_netpbm( char *format, int width, int height, struct params *p ) {

  preamble_common( format, width, height, p );
  printf( "%d %d\n", width, height );
}

void preamble_netpgm( char *format, int width, int height, struct params *p ) {

  preamble_netpbm( format, width, height, p );
  printf( "%d\n", c_max );
}

void preamble_netppm( char *format, int width, int height, struct params *p ) {

  preamble_netpbm( format, width, height, p );
  printf( "%d\n", c_max );
}

void preamble_netpam( char *format, int width, int height, struct params *p ) {

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

void (*print_preamble)( char *format, int width, int height, struct params *p );
void (*print_color)( int iter, int max_iter );
void (*backend_p)( char *format, int width, int height, struct params *pp, int threads );

void backend_plain( char *format, int width, int height, struct params *pp, int threads ) {

  print_preamble( format, width, height, pp );

  double x_delta = ( pp->x_max - pp->x_min ) / width;
  double y_delta = ( pp->y_max - pp->y_min ) / height;
  double bailout = pp->bailout;
  int   max_iter = pp->max_iter;

  double b = pp->y_max;

  for( int j = 0; j < height; ++j, b -= y_delta ) {

    double a = pp->x_min;

    for( int i = 0; i < width; ++i, a += x_delta ) {

      int iter = 0;
      double x = 0.0, y = 0.0;

      while( ++iter < max_iter ) {

        double w  = x + y;
        double ww = w * w;
        double xx = x * x;
        double yy = y * y;
        double zz = xx + yy;

        if( zz > bailout ) break;

        x = a + xx - yy;
        y = b + ww - zz;
      }

      print_color( iter, max_iter );
    }
  }
}

struct thread_info {
  float a;
  float b;
  int iter;
  int max_iter;
  double bailout;
  pthread_t thread;
};

static void *thread_test_loop( void *arg ) {

  int iter = 0;
  double x = 0.0, y = 0.0;

  struct thread_info *item = arg;

  double a = item->a;
  double b = item->b;
  int max_iter = item->max_iter;
  double bailout  = item->bailout;

  while( ++iter < max_iter ) {

    double w  = x + y;
    double ww = w * w;
    double xx = x * x;
    double yy = y * y;
    double zz = xx + yy;

    if( zz > bailout ) break;

    x = a + xx - yy;
    y = b + ww - zz;
  }
  item->iter = iter;
}

// I actually managed to create a threaded implementation that is
// slower than the single-threaded version.  Yay me.  I did the
// easiest thing I could think of, and it was probably not the most
// efficient thing in terms of overhead (I constantly create new
// threads and then join them without ever reusing them).

void backend_threads_naive( char *format, int width, int height, struct params *pp, int threads ) {

  int q_size = 4; // this is hard-coded for now, until I get around to parameterizing it
  int q_used = 0; // number of queue slots in use
  int q_next = 0; // next queue slot to use

  struct thread_info *thread_queue = ( struct thread_info * )malloc( q_size * sizeof( struct thread_info ) );

  if( thread_queue == NULL ) {
    fprintf( stderr, "unable to allocate array of thread data - aborting\n" );
    exit( EXIT_FAILURE );
  }

  print_preamble( format, width, height, pp );

  double x_delta = ( pp->x_max - pp->x_min ) / width;
  double y_delta = ( pp->y_max - pp->y_min ) / height;
  double bailout = pp->bailout;
  int   max_iter = pp->max_iter;

  double b = pp->y_max;

  for( int j = 0; j < height; ++j, b -= y_delta ) {

    double a = pp->x_min;

    for( int i = 0; i < width; ++i, a += x_delta ) {

      if( q_used == q_size ) {

        int s = pthread_join( thread_queue[ q_next ].thread, NULL );
        if( s != 0 ) handle_error_en( s, "pthread_join" );

	--q_used;
	// TODO: check to ensure never less than zero

        print_color( thread_queue[ q_next ].iter, max_iter );
      }

      thread_queue[ q_next ].a = a;
      thread_queue[ q_next ].b = b;
      thread_queue[ q_next ].bailout  = bailout;
      thread_queue[ q_next ].max_iter = max_iter;

      int s = pthread_create( &thread_queue[ q_next ].thread, NULL, &thread_test_loop, &thread_queue[ q_next ] );
      if( s != 0 ) handle_error_en( s, "pthread_create" );

      ++q_used;
      // TODO: check to ensure never greater than q_size

      ++q_next;
      q_next %= q_size;
    }
  }
}

int main( int argc, char *argv[] ) {

  int threads = 1;
  char format[10] = "P6";
  char backend[20] = "plain";
  int width  = 1024;
  int height = 1024;
  struct params p;

  p.max_iter = 1000;
  p.bailout = 4.0;
  p.x_min = -2.0, p.x_max = +2.0;
  p.y_min = -2.0, p.y_max = +2.0;

  static struct option long_options[] = {
    {  "format", required_argument, 0, 'f' },
    { "Backend", required_argument, 0, 'B' },
    {   "width", required_argument, 0, 'w' },
    {  "height", required_argument, 0, 'h' },
    { "Maxiter", required_argument, 0, 'M' },
    { "bailout", required_argument, 0, 'b' },
    {      "mu", required_argument, 0, 'm' },
    {      "nu", required_argument, 0, 'n' },
    {    "xmin", required_argument, 0, 'x' },
    {    "Xmax", required_argument, 0, 'X' },
    {    "ymin", required_argument, 0, 'y' },
    {    "Ymax", required_argument, 0, 'Y' },
    { "threads", required_argument, 0, 't' },
    {         0,                 0, 0,  0  }
  };

  while( 1 ) {

    int c = getopt_long( argc, argv, "f:B:M:b:w:h:m:n:x:X:y:Y:t:", long_options, NULL );

    if( c < 0 ) break;

    switch( c ) {

    case 'f':
      sscanf( optarg, "%9s", format );
      break;

    case 'B':
      sscanf( optarg, "%19s", backend );
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

    case 'x':
      sscanf( optarg, "%9lf", &p.x_min );
      break;

    case 'X':
      sscanf( optarg, "%9lf", &p.x_max );
      break;

    case 'y':
      sscanf( optarg, "%9lf", &p.y_min );
      break;

    case 'Y':
      sscanf( optarg, "%9lf", &p.y_max );
      break;

    case 'm':
      sscanf( optarg, "%9d", &mu );
      break;

    case 'n':
      sscanf( optarg, "%9d", &nu );
      break;

    case 't':
      sscanf( optarg, "%9d", &threads );
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

  int arg_flag = 0;
  fn = strlen( backend );

  if( fn == 5 ) {
    if( 0 == strncmp( "plain", backend, fn ) ) {
      backend_p = &backend_plain;
      ++arg_flag;
    }
  }
  if( fn == 13 ) {
    if( 0 == strncmp( "threads_naive", backend, fn ) ) {
      backend_p = &backend_threads_naive;
      ++arg_flag;
    }
  }

  if( arg_flag == 0 ) {
    fprintf( stderr, "ERROR: specified backend \"%s\" is not recognized/supported\n", backend );
    exit( EXIT_FAILURE );
  }

  backend_p( format, width, height, &p, threads );
}
