// Polynominterpolation, check with https://www.wolframalpha.com
#include <stdio.h>
#define MAX_POINTS 10

double polynom(double x,const double xy[][2],int n) {
  double s=0,p;
    for(int i=n,j;i--;s+=p)
      for(p=xy[i][1],j=n;j--;)
        if(i!=j)p*=(x-xy[j][0])/(xy[i][0]-xy[j][0]);
return s;}

void main(void) {
  printf("InterpolatingPolynomial[{{4,0},{5.5,120},{20,1200}},x] at x=8 is 316.552\n");
  const double xy[MAX_POINTS][2]={{4,0},{5.5,120},{20,1200}};
  printf("My calc at x=8 is %f\n",polynom(8.0,xy,3));

  printf("InterpolatingPolynomial[{{0,0},{1,.8415},{2,.9093},{3,.1411},{4,-.7568},{5,-.9589}},x] at x=2.5 is 0.59596\n");
  const double xy2[MAX_POINTS][2] = {{0,0},{1,.8415},{2,.9093},{3,.1411},{4,-.7568},{5,-.9589}};
  printf("My calc at x=2.5 is %f\n", polynom(2.5, xy2, 6));

  printf("InterpolatingPolynomial[{{2,3},{4,7}},x] at x=3 is 5.0\n");
  const double xy3[MAX_POINTS][2] = {{2,3},{4,7}};
  printf("My calc at x=3 is %f\n", polynom(3.0, xy3, 2));

  printf("InterpolatingPolynomial[{{0,0},{1,1},{2,4},{3,9}},x] at x=1.5 is 2.25\n");
  const double xy4[MAX_POINTS][2] = {{0,0},{1,1},{2,4},{3,9}};
  printf("My calc at x=1.5 is %f\n", polynom(1.5, xy4, 4));

  printf("InterpolatingPolynomial[{{1,1},{2,8},{3,27},{4,64}},x] at x=2.5 is 15.625\n");
  const double xy5[MAX_POINTS][2] = {{1,1},{2,8},{3,27},{4,64}};
  printf("My calc at x=2.5 is %f\n", polynom(2.5, xy5, 4));

  printf("InterpolatingPolynomial[{{0,1},{1,2},{2,4},{3,8}},x] at x=1.5 is 2.8125\n");
  const double xy6[MAX_POINTS][2] = {{0,1},{1,2},{2,4},{3,8}};
  printf("My calc at x=1.5 is %f\n", polynom(1.5, xy6, 4));

  printf("InterpolatingPolynomial[{{-2,4},{-1,1},{0,0},{1,1},{2,4}},x] at x=1.5 is 2.25\n");
  const double xy7[MAX_POINTS][2] = {{-2,4},{-1,1},{0,0},{1,1},{2,4}};
  printf("My calc at x=1.5 is %f\n", polynom(1.5, xy7, 5));
}


