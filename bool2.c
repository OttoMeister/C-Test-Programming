#include <stdbool.h>
#include <stdio.h>

void
print_binary1 (unsigned char zahl)
{
  for (int i = 8; i > 0; i--) printf ("%d", (zahl & (1 << (i - 1))) > 0);
}

void
print_binary2 (unsigned char zahl)
{
  for (int i = 7; i >= 0; i--) putchar((zahl & (1u << i)) ? '1' : '0');
}

void print_binary3(unsigned char zahl)
{
  for (int i = 7; i >= 0; i--) putchar((zahl >> i) & 1u ? '1' : '0');
}

#define BIT4(pos) ( 1<<(pos) )	
#define CHECK_BIT4(y, pos) ( ( 0u == ( (y)&(BIT4(pos)) ) ) ?0u :1u )	
void
print_binary4 (unsigned char zahl)
{
  printf ("%d", (CHECK_BIT4 (zahl, 7)));
  printf ("%d", (CHECK_BIT4 (zahl, 6)));
  printf ("%d", (CHECK_BIT4 (zahl, 5)));
  printf ("%d", (CHECK_BIT4 (zahl, 4)));
  printf ("%d", (CHECK_BIT4 (zahl, 3)));
  printf ("%d", (CHECK_BIT4 (zahl, 2)));
  printf ("%d", (CHECK_BIT4 (zahl, 1)));
  printf ("%d", (CHECK_BIT4 (zahl, 0)));
}


#define BIT5(pos) ( 1u << (pos) )  
#define CHECK_BIT5(y, pos) ( ( (y) & BIT5(pos) ) ? 1u : 0u )  
void print_binary5(unsigned char zahl)
{
    for (int i = 7; i >= 0; i--)
        putchar(CHECK_BIT5(zahl, i) + '0');

}

int main(void) {
  unsigned char test_cases[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 255};
  for (int i = 0; i < sizeof(test_cases) / sizeof(unsigned char); i++) {
    printf("%03.3d in binary: ", test_cases[i]);
    print_binary1(test_cases[i]); printf(" - ");
    print_binary2(test_cases[i]); printf(" - ");   
    print_binary3(test_cases[i]); printf(" - ");   
    print_binary4(test_cases[i]); printf(" - ");   
    print_binary5(test_cases[i]); 
    putchar('\n');
  }
  return 0;
}
