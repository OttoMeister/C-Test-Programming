// Polynominterpolation, check with https://www.wolframalpha.com
// Lagrange-Interpolation: P(x) = Σᵢ yᵢ · ∏ⱼ≠ᵢ (x−xⱼ)/(xᵢ−xⱼ)

#include <stdio.h>
#define MAX_POINTS 10

// Lagrange-Interpolation - short
double polynom1(double x,const double xy[][2],int n) {
  double s=0,p;
  for(int i=n,j;i--;s+=p)
	for(p=xy[i][1],j=n;j--;)
      if(i!=j)p*=(x-xy[j][0])/(xy[i][0]-xy[j][0]);
return s;}

// Lagrange-Interpolation - fast
double polynom2(double x,const double xy[][2],int n) {
  double s=0,a,b;
  for(int i=0,j;i<n;s+=xy[i][1]*a/b,i++){
    for(a=b=1,j=0;j<i;j++)a*=x-xy[j][0],b*=xy[i][0]-xy[j][0];
    for(j=i+1;j<n;j++)a*=x-xy[j][0],b*=xy[i][0]-xy[j][0];}
return s;}

// Newtonsche Polynominterpolation
double polynom3(double x,const double xy[][2],int n){
  double c[n],s=0;
  for(int i=0;i<n;i++)c[i]=xy[i][1];
  for(int j=1;j<n;j++)
    for(int i=n-1;i>=j;i--)
      c[i]=(c[i]-c[i-1])/(xy[i][0]-xy[i-j][0]);
  for(int i=n-1;i>=0;i--)s=s*(x-xy[i][0])+c[i];
return s;}

//Neville-Aitken-Algorithmus
double polynom4(double x,const double xy[][2],int n){
  double p[n];
  for(int i=0;i<n;i++)p[i]=xy[i][1];
  for(int j=1;j<n;j++)
    for(int i=n-1;i>=j;i--)
      p[i]=((x-xy[i-j][0])*p[i]-(x-xy[i][0])*p[i-1])/(xy[i][0]-xy[i-j][0]);
  return p[n-1];}

// Kubische Spline-Interpolation
double spline(double x,const double xy[][2],int n){
  double h[n],a[n],c[n],m[n],z[n],dx,l;int i=0;
  for(;i<n-1;i++)h[i]=xy[i+1][0]-xy[i][0];
  for(i=1;i<n-1;i++)a[i]=3./h[i]*(xy[i+1][1]-xy[i][1])-3./h[i-1]*(xy[i][1]-xy[i-1][1]);
  for(m[0]=z[0]=0,i=1;i<n-1;i++)l=2.*(xy[i+1][0]-xy[i-1][0])-h[i-1]*m[i-1],m[i]=h[i]/l,z[i]=(a[i]-h[i-1]*z[i-1])/l;
  for(c[n-1]=0,i=n-2;i>=0;i--)c[i]=z[i]-m[i]*c[i+1];
  for(i=0;i<n-2&&x>xy[i+1][0];i++);
  dx=x-xy[i][0];
  return xy[i][1]+dx*((xy[i+1][1]-xy[i][1])/h[i]-h[i]*(c[i+1]+2*c[i])/3.+dx*(c[i]+dx*(c[i+1]-c[i])/(3.*h[i])));
}


void main(void) {
  printf("InterpolatingPolynomial[{{4,0},{5.5,120},{20,1200}},x] at x=8 is 316.552\n");
  const double xy[MAX_POINTS][2]={{4,0},{5.5,120},{20,1200}};
  printf("My calc at x=8 is %f\n",polynom1(8.0,xy,3));

  printf("InterpolatingPolynomial[{{0,0},{1,.8415},{2,.9093},{3,.1411},{4,-.7568},{5,-.9589}},x] at x=2.5 is 0.59596\n");
  const double xy2[MAX_POINTS][2] = {{0,0},{1,.8415},{2,.9093},{3,.1411},{4,-.7568},{5,-.9589}};
  printf("My calc at x=2.5 is %f\n", polynom2(2.5, xy2, 6));

  printf("InterpolatingPolynomial[{{2,3},{4,7}},x] at x=3 is 5.0\n");
  const double xy3[MAX_POINTS][2] = {{2,3},{4,7}};
  printf("My calc at x=3 is %f\n", polynom3(3.0, xy3, 2));

  printf("InterpolatingPolynomial[{{0,0},{1,1},{2,4},{3,9}},x] at x=1.5 is 2.25\n");
  const double xy4[MAX_POINTS][2] = {{0,0},{1,1},{2,4},{3,9}};
  printf("My calc at x=1.5 is %f\n", polynom4(1.5, xy4, 4));

  printf("InterpolatingPolynomial[{{1,1},{2,8},{3,27},{4,64}},x] at x=2.5 is 15.625\n");
  const double xy5[MAX_POINTS][2] = {{1,1},{2,8},{3,27},{4,64}};
  printf("My calc at x=2.5 is %f\n", polynom1(2.5, xy5, 4));

  printf("InterpolatingPolynomial[{{0,1},{1,2},{2,4},{3,8}},x] at x=1.5 is 2.8125\n");
  const double xy6[MAX_POINTS][2] = {{0,1},{1,2},{2,4},{3,8}};
  printf("My calc at x=1.5 is %f\n", polynom2(1.5, xy6, 4));

  printf("InterpolatingPolynomial[{{-2,4},{-1,1},{0,0},{1,1},{2,4}},x] at x=1.5 is 2.25\n");
  const double xy7[MAX_POINTS][2] = {{-2,4},{-1,1},{0,0},{1,1},{2,4}};
  printf("My calc at x=1.5 is %f\n", polynom3(1.5, xy7, 5));

  // Testing Cubic Spline Interpolation
  // Another test: Linear-ish data
  printf("Cubic Spline {{0,0},{1,2},{2,4}} at x=1.5 should be 3.0\n");
  const double xy_linear[MAX_POINTS][2] = {{0,0}, {1,2}, {2,4}};
  printf("My spline calc at x=1.5 is %f\n", spline(1.5, xy_linear, 3));

  // --- The "Runge's Phenomenon" Test ---
  // Points that are mostly flat but have a sharp peak in the middle.
  // Standard polynomials will "wiggle" (overshoot) at the edges.
  printf("\n--- Sharp Peak Test (Runge's Phenomenon) ---\n");
  const double xy_peak[MAX_POINTS][2] = {
    {0, 0}, {1, 0.1}, {2, 0.2}, {3, 1.0}, {4, 0.2}, {5, 0.1}, {6, 0}
  };
  double x_test = 0.5; // Near the edge
  double p_val = polynom1(x_test, xy_peak, 7);
  double s_val = spline(x_test, xy_peak, 7);
  printf("At x=%.1f (Expected near 0.05):\n", x_test);
  printf("  Lagrange Poly: %f (Notice the 'ringing' error!)\n", p_val);
  printf("  Cubic Spline : %f (Much more stable)\n", s_val);

}


