// Easy fast 32bit Permuted Congruential Generator
// *Really* minimal PCG32 code / (c) 2014 M.E. O'Neill / pcg-random.org
// Licensed under Apache License 2.0 (NO WARRANTY, etc. see website) 


// https://onlinegdb.com/eaBVCCU48
// https://formatter.org/cpp-formatter

// Easy fast 32bit Permuted Congruential Generator
// *Really* minimal PCG32 code / (c) 2014 M.E. O'Neill / pcg-random.org
// Licensed under Apache License 2.0 (NO WARRANTY, etc. see website)

// https://onlinegdb.com/Q6efdM15N
// https://formatter.org/cpp-formatter

#include <stdint.h>
#include <stdio.h>
#include <time.h>

typedef struct {
  uint64_t state;
  uint64_t inc;
} pcg32_random_t;

uint32_t pcg32_random_r(pcg32_random_t *rng) {
  uint64_t oldstate = rng->state;
  rng->state = oldstate * 6364136223846793005ULL + (rng->inc | 1);
  uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
  uint32_t rot = oldstate >> 59u;
  return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

void pcg32_srandom_r(pcg32_random_t *rng, uint64_t initstate,
                     uint64_t initseq) {
  rng->state = 0U;
  rng->inc = (initseq << 1u) | 1u;
  pcg32_random_r(rng);
  rng->state += initstate;
  pcg32_random_r(rng);
}

int main(void) {
  pcg32_random_t rng = {0x853c49e6748fea9bULL, 0xda3e39cb94b95bdbULL};
  // nondeterministic seed
  pcg32_srandom_r(&rng, time(NULL) ^ (intptr_t)&printf, (intptr_t)&rng);
  printf("Some 32-bit numbers:\n");
  for (int i = 0; i < 5; ++i) {
    for (int ii = 0; ii < 6; ++ii)
      printf("%08x ", pcg32_random_r(&rng));
    printf("\n");
  }
  return 0;
}
