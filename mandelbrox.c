#include <math.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <pthread.h>
#include <unistd.h>

#ifdef DEBUG
# define DPRINTF(...) printf(__VA_ARGS__)
#else
# define DPRINTF(...)
#endif

// I lifted this directly from the PTHREAD_CREATE(3) manpage
#define handle_error_en(en, msg) \
   do { errno = en; perror( msg ); exit( EXIT_FAILURE ); } while (0)

int c_max = (1<<8) - 1;

int mu = 2;
int nu = 50;

double sixth = M_PI / 3.0;

typedef struct color {
  double red;
  double green;
  double blue;
} color_t, *color_p;

typedef struct params {
  double x_min;
  double x_max;
  double y_min;
  double y_max;
  double bailout;
  int max_iter;
} params_t, *params_p;

typedef struct work_unit {
  double a;
  double b;
  int iter;
  int max_iter;
  double bailout;
  pthread_t thread;
  pthread_mutex_t mutex;
  pthread_cond_t start;
  pthread_cond_t finish;
  int busy;
  struct work_unit *next;
} work_unit_t, *work_unit_p;

void preamble_common( char *format, int width, int height, params_p pp ) {

  printf( "%s\n", format );
  printf( "# x_min = %f\n",    pp->x_min );
  printf( "# x_max = %f\n",    pp->x_max );
  printf( "# y_min = %f\n",    pp->y_min );
  printf( "# y_max = %f\n",    pp->y_max );
  printf( "# bailout = %f\n",  pp->bailout );
  printf( "# max_iter = %d\n", pp->max_iter );
}

void preamble_netpbm( char *format, int width, int height, params_p pp ) {

  preamble_common( format, width, height, pp );
  printf( "%d %d\n", width, height );
}

void preamble_netpgm( char *format, int width, int height, params_p pp ) {

  preamble_netpbm( format, width, height, pp );
  printf( "%d\n", c_max );
}

void preamble_netppm( char *format, int width, int height, params_p pp ) {

  preamble_netpbm( format, width, height, pp );
  printf( "%d\n", c_max );
}

void preamble_netpam( char *format, int width, int height, params_p pp ) {

  preamble_common( format, width, height, pp );
  printf( "WIDTH %d\nHEIGHT %d\nMAXVAL %d\n", width, height, c_max );
  printf( "TUPLTYPE RGB\nDEPTH 3\nENDHDR\n" );
}

void color_netpbm( int iter, int max_iter ) {
  printf( ( iter < max_iter ) ? "0 " : "1 " );
}

void color_netpgm( int iter, int max_iter ) {
  printf( "%d ", ( c_max * ( max_iter - iter ) ) / max_iter );
}

void color_picker( int iter, int max_iter, color_p c ) {

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

  color_t c;

  color_picker( iter, max_iter, &c );
  printf( "%d %d %d ", (int)c.red, (int)c.green, (int)c.blue );
}

void color_netpam( int iter, int max_iter ) {

  color_t c;

  color_picker( iter, max_iter, &c );
  printf( "%c%c%c", (char)c.red, (char)c.green, (char)c.blue );
}

void (*print_preamble)( char *format, int width, int height, params_p pp );
void (*print_color)( int iter, int max_iter );
void (*backend)( char *format, int width, int height, params_p pp, int threads );

static void *worker_loop( void *arg ) {

  work_unit_p item = arg;

  double a = item->a;
  double b = item->b;
  double bailout  = item->bailout;
  int max_iter = item->max_iter;

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
  item->iter = iter;
}

static void *worker_wrapper( void *arg ) {

  work_unit_p wup = (work_unit_p)arg;

  while( 1 ) {

    int s = pthread_mutex_lock( &wup->mutex );
    if( s != 0 ) handle_error_en( s, "pthread_mutex_lock" );

    DPRINTF( stderr, "DEBUG: thread %p waiting to receive start signal from main thread\n", &(wup->thread) );

    while( wup->busy ) {
      s = pthread_cond_wait( &wup->start, &wup->mutex );
      if( s != 0 ) handle_error_en( s, "pthread_cond_wait" );
    }

    DPRINTF( stderr, "DEBUG: thread %p received start signal from main thread\n", &(wup->thread) );

    wup->busy = 1;

    s = pthread_mutex_unlock( &wup->mutex );
    if( s != 0 ) handle_error_en( s, "pthread_mutex_unlock" );

    worker_loop( arg );

    s = pthread_mutex_lock( &wup->mutex );
    if( s != 0 ) handle_error_en( s, "pthread_mutex_lock" );

    DPRINTF( stderr, "DEBUG: thread %p preparing to send finish signal to main thread\n", &(wup->thread) );

    s = pthread_cond_signal( &wup->finish );
    if( s != 0 ) handle_error_en( s, "pthread_cond_signal" );

    DPRINTF( stderr, "DEBUG: thread %p sent finish signal to main thread\n", &(wup->thread) );

    s = pthread_mutex_unlock( &wup->mutex );
    if( s != 0 ) handle_error_en( s, "pthread_mutex_unlock" );
  }
}

void backend_plain( char *format, int width, int height, params_p pp, int threads ) {

  print_preamble( format, width, height, pp );

  double x_delta = ( pp->x_max - pp->x_min ) / width;
  double y_delta = ( pp->y_max - pp->y_min ) / height;
  double bailout = pp->bailout;
  int   max_iter = pp->max_iter;

  double b = pp->y_max;

  for( int j = 0; j < height; ++j, b -= y_delta ) {

    double a = pp->x_min;

    for( int i = 0; i < width; ++i, a += x_delta ) {

      work_unit_t wu;

      wu.a = a;
      wu.b = b;
      wu.bailout  = bailout;
      wu.max_iter = max_iter;

      worker_loop( &wu );

      print_color( wu.iter, wu.max_iter );
    }
  }
}

// I actually managed to create a threaded implementation that is
// slower than the single-threaded version.  Yay me.  I did the
// easiest thing I could think of, and it was probably not the most
// efficient thing in terms of overhead (I constantly create new
// threads and then join them without ever reusing them).

void backend_threads_naive( char *format, int width, int height, params_p pp, int threads ) {

  int q_size = threads; // this is hard-coded for now, until I get around to parameterizing it
  int q_used = 0; // number of queue slots in use
  int q_next = 0; // next queue slot to use

  work_unit_p work_queue = ( work_unit_p )malloc( q_size * sizeof( work_unit_t ) );

  if( work_queue == NULL ) {
    DPRINTF( stderr, "unable to allocate array of worker data - aborting\n" );
    exit( EXIT_FAILURE );
  }

  work_unit_p wup = work_queue;

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

        int s = pthread_join( wup->thread, NULL );
        if( s != 0 ) handle_error_en( s, "pthread_join" );

	--q_used;
	// TODO: check to ensure never less than zero

        print_color( wup->iter, max_iter );
      }

      wup->a = a;
      wup->b = b;
      wup->bailout  = bailout;
      wup->max_iter = max_iter;

      int s = pthread_create( &(wup->thread), NULL, &worker_loop, wup );
      if( s != 0 ) handle_error_en( s, "pthread_create" );

      ++q_used;
      // TODO: check to ensure never greater than q_size

      ++q_next;
      q_next %= q_size;

      wup = &(work_queue[ q_next ]);
    }
  }
}

void backend_threads_simple( char *format, int width, int height, params_p pp, int threads ) {

  int q_size = threads; // this is hard-coded for now, until I get around to parameterizing it
  int q_used = 0; // number of queue slots in use
  int q_next = 0; // next queue slot to use

  int s;

  work_unit_p work_ring;
  work_unit_p *wupp = &work_ring;

  for( int t = 0; t < q_size; ++t ) {

    work_unit_p new = (work_unit_p)malloc( sizeof( work_unit_t ) );

    s = pthread_mutex_init( &(new->mutex), NULL );
    if( s != 0 ) handle_error_en( s, "pthread_mutex_init" );

    s = pthread_cond_init( &(new->start), NULL );
    if( s != 0 ) handle_error_en( s, "pthread_cond_init" );

    s = pthread_cond_init( &(new->finish), NULL );
    if( s != 0 ) handle_error_en( s, "pthread_cond_init" );

    s = pthread_create( &(new->thread), NULL, &worker_wrapper, (void *)new );
    if( s != 0 ) handle_error_en( s, "pthread_create" );

    *wupp = new;
    wupp = &(new->next);
  }

  *wupp = work_ring;

  work_unit_p wup = work_ring;

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

        int s = pthread_mutex_lock( &wup->mutex );
        if( s != 0 ) handle_error_en( s, "pthread_mutex_lock" );

        DPRINTF( stderr, "DEBUG: main thread preparing to wait for finish signal from thread %p\n", &(wup->thread) );

	while( ! wup->busy ) {
          s = pthread_cond_wait( &wup->finish, &wup->mutex );
          if( s != 0 ) handle_error_en( s, "pthread_cond_wait" );
	}

        wup->busy = 0;

        DPRINTF( stderr, "DEBUG: main thread received finish signal from thread %p\n", &(wup->thread) );

        s = pthread_mutex_unlock( &wup->mutex );
        if( s != 0 ) handle_error_en( s, "pthread_mutex_unlock" );

        print_color( wup->iter, max_iter );

	--q_used;
      }

      wup->a = a;
      wup->b = b;
      wup->bailout  = bailout;
      wup->max_iter = max_iter;

      int s = pthread_mutex_lock( &wup->mutex );
      if( s != 0 ) handle_error_en( s, "pthread_mutex_lock" );

      DPRINTF( stderr, "DEBUG: main thread preparing to send start signal to thread %p\n", &(wup->thread) );

      s = pthread_cond_signal( &wup->start );
      if( s != 0 ) handle_error_en( s, "pthread_cond_signal" );

      DPRINTF( stderr, "DEBUG: main thread sent start signal to thread %p\n", &(wup->thread) );

      s = pthread_mutex_unlock( &wup->mutex );
      if( s != 0 ) handle_error_en( s, "pthread_mutex_unlock" );

      ++q_used;
      // TODO: check to ensure never greater than q_size

      wup = wup->next;
    }
  }
}

int main( int argc, char *argv[] ) {

  int threads = 1;
  char format[10] = "P6";
  char backend_name[20] = "plain";
  int width  = 1024;
  int height = 1024;
  params_t p;

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
      sscanf( optarg, "%19s", backend_name );
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
  fn = strlen( backend_name );

  if( fn == 5 ) {
    if( 0 == strncmp( "plain", backend_name, fn ) ) {
      backend = &backend_plain;
      ++arg_flag;
    }
  }
  if( fn == 13 ) {
    if( 0 == strncmp( "threads_naive", backend_name, fn ) ) {
      backend = &backend_threads_naive;
      ++arg_flag;
    }
  }
  if( fn == 14 ) {
    if( 0 == strncmp( "threads_simple", backend_name, fn ) ) {
      backend = &backend_threads_simple;
      ++arg_flag;
    }
  }

  if( arg_flag == 0 ) {
    fprintf( stderr, "ERROR: specified backend \"%s\" is not recognized/supported\n", backend_name );
    exit( EXIT_FAILURE );
  }

  backend( format, width, height, &p, threads );
}
