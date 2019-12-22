#include <stdio.h>
#include <getopt.h>

int main( int argc, char *argv[] ) {

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

    int option_index = 0;

    int c = getopt_long( argc, argv, "m:b:w:h:", long_options, &option_index );

    if( c < 0 ) break;

    switch( c ) {
    case 0:
      printf( "option %s", long_options[option_index].name );
      if( optarg ) printf(" with arg %s", optarg);
      printf( "\n" );
      break;

    case 'm':
      sscanf( optarg, "%d", &max_iter );
      break;

    case 'w':
      sscanf( optarg, "%d", &width );
      break;

    case 'h':
      sscanf( optarg, "%d", &height );
      break;

    case 'b':
      sscanf( optarg, "%lf", &bailout );
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
    fprintf( stderr, "\n");
  }

  printf( "P1\n%d %d\n", width, height );
  printf( "# x_min = %f\n", x_min );
  printf( "# x_max = %f\n", x_max );
  printf( "# y_min = %f\n", y_min );
  printf( "# y_max = %f\n", y_max );
  printf( "# max_iter = %d\n", max_iter );

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
