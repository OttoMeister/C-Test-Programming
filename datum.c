// date and time test esp32 code on linux pc: gcc -o datum datum.c && ./datum
#include <stdio.h>
#include <stdint.h>
#include <time.h>

const char* getFormattedDate(uint32_t et) {
  uint32_t zz,er,de,ye,yy,dy,mp;
  uint8_t dd,mm;static char bf[11];
  zz=et/86400+719468;er=zz/146097;de=zz-er*146097;
  ye=(de-de/1460+de/36524-de/146096)/365;yy=ye+er*400;
  dy=de-(365*ye+ye/4-ye/100);mp=(5*dy+2)/153;
  dd=dy-(153*mp+2)/5+1;mm=mp<10?mp+3:mp-9;yy+=(mm<=2);
  bf[0]='0'+dd/10;bf[1]='0'+dd%10;bf[2]='-';
  bf[3]='0'+mm/10;bf[4]='0'+mm%10;bf[5]='-';
  bf[6]='0'+yy/1000;bf[7]='0'+(yy/100)%10;
  bf[8]='0'+(yy/10)%10;bf[9]='0'+yy%10;bf[10]='\0';
  return bf;
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
