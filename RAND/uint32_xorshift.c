// uint32 Xorshift random number generators
// More on: https://en.wikipedia.org/wiki/Xorshift

// https://onlinegdb.com/wwVW_btuI
// https://formatter.org/cpp-formatter

#include <stdint.h>
#include <stdio.h>
#include <time.h>

typedef struct {
  uint32_t w;
  uint32_t x;
  uint32_t y;
  uint32_t z;
  uint32_t t;
} XORShiftParameters;

uint32_t xorshift128(XORShiftParameters* params) {
  uint32_t t = params->x;
  t ^= t << 11U;
  t ^= t >> 8U;
  params->x = params->y;
  params->y = params->z;
  params->z = params->w;
  params->w ^= params->w >> 19U;
  params->w ^= t;
  return params->w;
}

void initializeStartValues(XORShiftParameters* params) {
  time_t currentTime;
  time(&currentTime);
  uint32_t address1 = (intptr_t)&initializeStartValues;
  uint32_t address2 = (intptr_t)&xorshift128;
  uint32_t address3 = (intptr_t)&params;
  params->w = (uint32_t)currentTime;
  params->x = (params->w ^ address1) + 1;
  params->y = (params->x ^ address2) - 1;
  params->z = (params->y ^ address3) * 2;
  params->t = 0;
}

int main(void) {
  XORShiftParameters parameters = {1, 2, 3, 4, 0};
  initializeStartValues(&parameters);
  for (int i = 0; i < 1000; ++i) xorshift128(&parameters);
  printf("Some 32-bit numbers:\n");
  for (int i = 0; i < 5; ++i) {
    for (int ii = 0; ii < 6; ++ii) printf("%08x ", xorshift128(&parameters));
    printf("\n");
  }
  return 0;
}
