// Easy fast 64bit Permuted Congruential Generator
// *Really* minimal PCG32 code / (c) 2014 M.E. O'Neill / pcg-random.org
// Licensed under Apache License 2.0 (NO WARRANTY, etc. see website)

// https://onlinegdb.com/xb-40kAm8L
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
} pcg32_random;

typedef struct {
  pcg32_random pcg32_random1;
  pcg32_random pcg32_random2;
} pcg64_random;

uint32_t pcg32_random_r(pcg32_random* rng) {
  uint64_t oldstate = rng->state;
  rng->state = oldstate * 6364136223846793005ULL + (rng->inc | 1);
  uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
  uint32_t rot = oldstate >> 59u;
  return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

uint64_t pcg64_random_r(pcg64_random* rng) {
  uint32_t a = pcg32_random_r(&rng->pcg32_random1);
  uint32_t b = pcg32_random_r(&rng->pcg32_random2);
  return ((uint64_t)a << 32) | b;
}

void pcg32_srandom_r(pcg32_random* rng, uint64_t initstate, uint64_t initseq) {
  rng->state = 0U;
  rng->inc = (initseq << 1u) | 1u;
  pcg32_random_r(rng);
  rng->state += initstate;
  pcg32_random_r(rng);
}

void pcg64_srandom_r(pcg64_random* rng, uint64_t initstate, uint64_t initseq) {
  pcg32_srandom_r(&rng->pcg32_random1, initstate + 1, initseq + 1);
  uint64_t initseq2 = ~initseq;
  uint64_t initstate2 = ~initstate;
  pcg32_srandom_r(&rng->pcg32_random2, initstate2 + 2, initseq2 + 2);
}

int main(void) {
  pcg32_random rng32;
  pcg64_random rng64;
  pcg32_srandom_r(&rng32, 42u, 54u);
  pcg64_srandom_r(&rng64, 42u, 54u);
  // nondeterministic seed
  pcg32_srandom_r (&rng32, time (NULL) ^ (intptr_t) & printf, (intptr_t) & rng32); 
  pcg64_srandom_r (&rng64, time (NULL) ^ (intptr_t) & printf, (intptr_t) & rng64);
  printf("Some 32-bit numbers:\n");
  for (int i = 0; i < 5; ++i) {
    for (int ii = 0; ii < 6; ++ii) printf("%08x ", pcg32_random_r(&rng32));
    printf("\n");
  }
  printf("Some 64-bit numbers:\n");
  for (int i = 0; i < 5; ++i) {
    for (int ii = 0; ii < 6; ++ii) printf("%016lx ", pcg64_random_r(&rng64));
    printf("\n");
  }
  return 0;
}
