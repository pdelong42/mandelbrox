#include <stdio.h>

int main( int argc, char *argv[] ) {

  int width = 1000;
  int height = 1000;
  int max_iter = 1000;
  double x_min = -2.0;
  double x_max = +2.0;
  double y_min = -2.0;
  double y_max = +2.0;

  for( int j = 0; j < height; ++j ) {
    for( int i = 0; i < width; ++i ) {
      printf( "(%d, %d)\n", i, j );
    }
  }
}
