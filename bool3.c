
#include <stdbool.h>
#include <stdio.h>

typedef struct onebyte
{
  union
  {
    unsigned char byte;
    struct
    {
      bool bit0:1;
      bool bit1:1;
      bool bit2:1;
      bool bit3:1;
      bool bit4:1;
      bool bit5:1;
      bool bit6:1;
      bool bit7:1;
    };
  };
} onebyte;

void
print_binary (unsigned char zahl)
{
  for (int i = 7; i >= 0; i--)
    putchar ((zahl >> i) & 1u ? '1' : '0');
}

int
main ()
{
  onebyte test;
  unsigned char e = 255;
  test.bit0 = 1;
  test.bit1 = 0;
  test.bit2 = 1;
  test.bit3 = 1;
  test.bit4 = 0;
  test.bit5 = 1;
  test.bit6 = 1;
  test.bit7 = 0;
  e = test.byte;
  print_binary (e);
  putchar ('\n');
  return 0;
}
