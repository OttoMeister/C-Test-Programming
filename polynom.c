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
}
