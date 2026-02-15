// x.com/yuruyurau/status/2022526453779435912
// gcc -o yuruyurau yuruyurau.c -lSDL2 -lSDL2_gfx -lm  && ./yuruyurau

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <math.h>

#define W 800
#define H 800

void h2r(float h,Uint8*r,Uint8*g,Uint8*b){float c=.8,x=c*(1-fabs(fmod(h/60,2)-1)),m=.2,r1,g1,b1;if(h<60){r1=c;g1=x;b1=0;}else if(h<120){r1=x;g1=c;b1=0;}else if(h<180){r1=0;g1=c;b1=x;}else if(h<240){r1=0;g1=x;b1=c;}else if(h<300){r1=x;g1=0;b1=c;}else{r1=c;g1=0;b1=x;}*r=(r1+m)*255;*g=(g1+m)*255;*b=(b1+m)*255;}

int main(){
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window*w=SDL_CreateWindow("",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,W,H,0);
    SDL_Renderer*d=SDL_CreateRenderer(w,-1,SDL_RENDERER_ACCELERATED);
    SDL_Event e;float t=0;int r=1,i;float s=fmin(W,H)/400.0,y,k,E,o,q,c,px,py,sx,sy;Uint8 R,G,B;
    while(r){
        while(SDL_PollEvent(&e))if(e.type==SDL_QUIT||e.type==SDL_KEYDOWN&&e.key.keysym.sym==SDLK_ESCAPE)r=0;
        SDL_SetRenderDrawColor(d,0,0,0,255);SDL_RenderClear(d);t+=M_PI/30;
        for(i=0;i<20000;i++){
            y=i/500.0;k=cos(y*9)*(y<5?sin(t/8+y)*35:11);E=y/8-13;o=sqrt(k*k+E*E)/6;q=k*y/19+49+k*sin(y)*sin(o*2-E/5-t);
            c=o/3-E/5-t/8+(i%4)*8;px=q*sin(c)-79*cos(c/3)+200;py=200+(q+70)*cos(c);
            sx=(px-200)*s+W/2;sy=(py-200)*s+H/2;h2r(fmod(t*30+y*20+c*10,360),&R,&G,&B);
            pixelRGBA(d,(int)sx,(int)sy,R,G,B,120);
        }
        SDL_RenderPresent(d);SDL_Delay(16);
    }
    SDL_DestroyRenderer(d);SDL_DestroyWindow(w);SDL_Quit();return 0;
}

