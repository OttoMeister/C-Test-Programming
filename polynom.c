// Polynominterpolation, check with https://www.wolframalpha.com

#include <stdio.h>
#define MAX_POINTS 10
double polynom(double x, const double xy[][2], int pts) {
  double w[MAX_POINTS][2];
  for(int i=0;i<pts;++i){w[i][0]=xy[i][0];w[i][1]=xy[i][1];}
  for(int i=1;i<pts;++i)for(int j=0;j<pts-i;++j)
    w[j][1]=((x-w[j+i][0])*w[j][1]-(x-w[j][0])*w[j+1][1])/(w[j][0]-w[j+i][0]);
  return w[0][1];
}
void main(void) {
  printf("InterpolatingPolynomial[{{4,0},{5.5,120},{20,1200}},x] at x=8 is 316.552\n");
  const double xy[MAX_POINTS][2]={{4,0},{5.5,120},{20,1200}};
  printf("InterpolatingPolynomial[{{4,0},{5.5,120},{20,1200}},x] at x=8 is %f\n",polynom(8.0,xy,3));

  printf("InterpolatingPolynomial[{{0,0},{1,0.8415},{2,0.9093},{3,0.1411},{4,-0.7568},{5,-0.9589}},x] at x=2.5 is 0.59596\n");
  const double xy2[MAX_POINTS][2] = {{0.0,0.0000},{1.0,0.8415},{2.0,0.9093},{3.0,0.1411},{4.0,-0.7568},{5.0,-0.9589}};
  printf("InterpolatingPolynomial[{{0,0},{1,0.8415},{2,0.9093},{3,0.1411},{4,-0.7568},{5,-0.9589}},x] at x=2.5 is %f\n", polynom(2.5, xy2, 6));
}









