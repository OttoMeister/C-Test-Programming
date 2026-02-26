// date and time test esp32 code on linux pc: gcc -o datum datum.c && ./datum

#include <stdio.h>
#include <stdint.h>
#include <time.h>

const char* getFormattedDate(uint32_t t) {
  static char b[11];uint32_t z,e,c,k,y,l,p;uint8_t d,m;
  z=t/86400+719468;e=z/146097;c=z-e*146097;
  k=(c-c/1460+c/36524-c/146096)/365;y=k+e*400;
  l=c-(365*k+k/4-k/100);p=(5*l+2)/153;
  d=l-(153*p+2)/5+1;m=p<10?p+3:p-9;y+=(m<=2);
  b[0]='0'+d/10;b[1]='0'+d%10;b[2]='-';
  b[3]='0'+m/10;b[4]='0'+m%10;b[5]='-';
  b[6]='0'+y/1000;b[7]='0'+(y/100)%10;
  b[8]='0'+(y/10)%10;b[9]='0'+y%10;b[10]='\0';
  return b;
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
