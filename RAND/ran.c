#include <stdint.h>
#include <stdio.h>

uint8_t za, zb, zc, zx;

#define randRNG8() ((za = (za ^ zc ^ zx), zb = (zb + za), zc = ((zc + (zb >> 1)) ^ za), zx++, zc))
#define randRNG16() ((uint16_t)(((uint16_t)randRNG8() << 8) | randRNG8()))
#define randRNG32() ((uint32_t)(((uint32_t)randRNG16() << 16) | randRNG16()))

void main(void) {
  za = 0;  zb = 0;  zc = 0;  zx = 0;
  
  for (int i = 0; i < 1000; i++) randRNG8();
  
  printf("8 Bit  = ");
  for (int i = 0; i < 15; i++) printf("%u ", randRNG8());

  printf("16 Bit = "); 
  for (int i = 0; i < 10; i++) printf("%u ", randRNG16()); 

  printf("32 Bit = "); 
  for (int i = 0; i < 5; i++) printf("%u ", randRNG32()); 

  printf("Roll the dice: "); 
  for (int i = 0; i < 25; i++) {
    uint8_t roll1 = randRNG32() % 6 + 1;
    printf("%u ", roll1); 
  }  
}
