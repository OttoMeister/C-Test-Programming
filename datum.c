// date and time test esp32 code on linux pc: gcc -o datum datum.c && ./datum

#include <stdio.h>
#include <stdint.h>
#include <time.h>

const char* getFormattedDate(uint32_t epochTime) {
  uint32_t z,era,doe,yoe,y,doy,mp;
  uint8_t day,mon;static char buf[11];
  z=epochTime/86400+719468;era=z/146097;doe=z-era*146097;
  yoe=(doe-doe/1460+doe/36524-doe/146096)/365;y=yoe+era*400;
  doy=doe-(365*yoe+yoe/4-yoe/100);mp=(5*doy+2)/153;
  day=doy-(153*mp+2)/5+1;mon=mp<10?mp+3:mp-9;y+=(mon<=2);
  buf[0]='0'+day/10; buf[1]='0'+day%10; buf[2]='-';
  buf[3]='0'+mon/10; buf[4]='0'+mon%10; buf[5]='-';
  buf[6]='0'+y/1000; buf[7]='0'+(y/100)%10;
  buf[8]='0'+(y/10)%10; buf[9]='0'+y%10; buf[10]='\0';
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

