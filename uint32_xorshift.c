// uint32 Xorshift random number generators
// More on: https://en.wikipedia.org/wiki/Xorshift

// https://onlinegdb.com/NRCvnT9KuT
// https://formatter.org/cpp-formatter

#include <stdint.h>
#include <stdio.h>
#include <time.h>

uint32_t w = 1, x = 2, y = 3, z = 4;

uint32_t xorshift128 (void)
{
  uint32_t t = x;
  t ^= t << 11U;
  t ^= t >> 8U;
  x = y;
  y = z;
  z = w;
  w ^= w >> 19U;
  w ^= t;
  return w;
}

int main (void)
{
  for (int i = 0; i < 1000; ++i) xorshift128 ();
  printf ("Some 32-bit numbers:\n");
  for (int i = 0; i < 5; ++i)
    {
      for (int ii = 0; ii < 6; ++ii) printf ("%08x ", xorshift128 ());
      printf ("\n");
    }
  return 0;
}
