// thx x.com/yuruyurau/status/2022526453779435912
// gcc -o yuruyurau yuruyurau.c -lSDL2 -lSDL2_gfx -lm  && ./yuruyurau
// apt install libstb-dev

#include <SDL2/SDL2_gfxPrimitives.h>
#define P(x,y) pixelRGBA(r,x,y,255,255,255,255)
SDL_Window*w;SDL_Renderer*r;SDL_Event e;
float t,k,E,o,q,c,y;
void a(int i){
  y=i/500.;o=hypot(k=cos(y*9)*(y<5?sin(t/8+y)*35:11),E=y/8-13)/6;
  q=k*y/19+49+k*sin(y)*sin(o*2-E/5-t);c=o/3-E/5-t/8+i%4*8;
  P((q*sin(c)-79*cos(c/3))*2+400,(q+70)*cos(c)*2+400);}
int main(){
  SDL_CreateWindowAndRenderer(800,800,0,&w,&r);
  for(;;){while(SDL_PollEvent(&e))if(e.type==SDL_QUIT)return 0;
    SDL_SetRenderDrawColor(r,0,0,0,255);SDL_RenderClear(r);
    for(int i=2e4;i--;)a(i);t+=.1;SDL_RenderPresent(r);SDL_Delay(32);}}

