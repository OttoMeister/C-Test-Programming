#include <stdio.h>
#include <math.h>
#define MAX_POINTS 10
// https://onlinegdb.com/dwC44b74B

// polynomial of degree 2 using Newton-Gregory forward interpolation 
double
polynom (double x, double xy[MAX_POINTS][2], int pts)
{
  int i, j, tpts;
  tpts = fmin (pts, MAX_POINTS);
  for (i = 1; i <= tpts; ++i)
    for (j = 0; j < tpts - i; ++j)
      xy[j][1] =
	((x - xy[j + i][0]) * xy[j][1] -
	 (x - xy[j][0]) * xy[j + 1][1]) / (xy[j][0] - xy[j + i][0]);
  return xy[0][1];
}

void
main ()
{
  // Test data using https://www.wolframalpha.com
  // quadratic fit {{4, 0}, {5.5, 120}, {20, 1200}}
  // -0.344828 x^2 + 83.2759 x - 327.586 ; x = 8 
  // result 316.552
  int pts = 3;
  double x = 8;
  double xy[MAX_POINTS][2] = { {4.0, 0.0}, {5.5, 120}, {20, 1200} };
  printf ("The result is: %f\n", polynom (x, xy, pts));
}
