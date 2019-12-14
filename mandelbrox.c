#include <stdio.h>

int main( int argc, char *argv[] ) {

  int width = 100;
  int height = 100;
  int max_iter = 1000;
  double x_min = -2.0;
  double x_max = +2.0;
  double y_min = -2.0;
  double y_max = +2.0;

  double x_ratio = ( x_max - x_min ) / width;
  double y_ratio = ( y_max - y_min ) / height;

  for( int j = 0; j < height; ++j ) {

    for( int i = 0; i < width; ++i ) {

      double x = x_min + i * x_ratio;
      double y = y_min + j * y_ratio;

      printf( "(%f, %f)\n", x, y );
    }
  }
}
