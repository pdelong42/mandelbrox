#include <stdio.h>

int main( int argc, char *argv[] ) {

  int width = 100;
  int height = 50;
  int max_iter = 1000;
  double x_min = -2.0, x_max = +2.0;
  double y_min = -2.0, y_max = +2.0;
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

        if( xx + yy > 4 ) break;

        double x_new = a + xx - yy;
        double y_new = b + 2 * x * y;

        x = x_new, y = y_new;
      }

      printf( ( iter < max_iter ) ? "X" : " " );
    }
    printf( "\n" );
  }
}
