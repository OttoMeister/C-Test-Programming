#include <stdint.h>
#include <stdio.h>


// https://eternityforest.com/doku/doku.php?id=tech:the_xabc_random_number_generator
uint8_t za, zb, zc, zx;
#define randRNG8() ((za = (za ^ zc ^ zx), zb = (zb + za), zc = ((zc + (zb >> 1)) ^ za), zx++, zc))
#define randRNG16() ((uint16_t)(((uint16_t)randRNG8() << 8) | randRNG8()))
#define randRNG32() ((uint32_t)(((uint32_t)randRNG16() << 16) | randRNG16()))


void
main (void)
{
  for (int i = 0; i < 1000; i++) randRNG8 ();
// https://manpages.ubuntu.com/manpages/trusty/man1/dieharder.1.html
  printf
    ("#==================================================================\n");
  printf ("# generator mt19937_1999  seed = 1274511046\n");
  printf
    ("#==================================================================\n");
  printf ("type: d\n");
  printf ("count: 100000\n");
  printf ("numbit: 32\n");
  for (int i = 0; i < 100000; i++)
    printf ("%u\n", randRNG32 ());
  printf ("\n");
}
