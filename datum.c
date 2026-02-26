// date and time test esp32 code on linux pc: gcc -o datum datum.c && ./datum

#include <stdio.h>
#include <stdint.h>
#include <time.h>

#define LEAP_YEAR(Y) (!(Y%4)&&((Y%100)||!(Y%400)))
const char* getFormattedDate(uint32_t epochTime){
  static const uint8_t md[]={31,28,31,30,31,30,31,31,30,31,30,31};
  uint16_t d=epochTime/86400UL,y=1970,ty; 
  uint8_t m=0,day,mon,tm;
  for(;d>=(ty=LEAP_YEAR(y)?366:365);d-=ty,y++);
  for(;m<12&&d>=(tm=m==1&&LEAP_YEAR(y)?29:md[m]);d-=tm,m++);
  static char buf[11];day=d+1;mon=m+1;
  buf[0]='0'+day/10;buf[1]='0'+day%10;buf[2]='-';
  buf[3]='0'+mon/10;buf[4]='0'+mon%10;buf[5]='-';
  buf[6]='0'+y/1000;buf[7]='0'+(y/100)%10;
  buf[8]='0'+(y/10)%10;buf[9]='0'+y%10;buf[10]='\0';
  return buf;
}

const char* getFormattedTime(uint32_t epochTime){
  static char buf[9];uint8_t h,m,s;
  h=(epochTime%86400UL)/3600;m=(epochTime%3600)/60;s=epochTime%60;
  buf[0]='0'+h/10;buf[1]='0'+h%10;buf[2]=':';
  buf[3]='0'+m/10;buf[4]='0'+m%10;buf[5]=':';
  buf[6]='0'+s/10;buf[7]='0'+s%10;buf[8]='\0';
  return buf;
}

void main(void){
    int32_t offsetSec = -5*3600;
    uint32_t epochTime=(uint32_t)time(NULL); // linux code
    printf("Epoch: %u, Offset: %i\n",epochTime,offsetSec);
    printf("Datum: %s\n",getFormattedDate(epochTime+offsetSec));
    printf("Zeit:  %s\n",getFormattedTime(epochTime+offsetSec));
}

