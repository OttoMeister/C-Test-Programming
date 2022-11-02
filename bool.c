// Online Compiler https://onlinegdb.com/Ui95yOqNo
// Online Compiler https://tinyurl.com/yc7ktjyx


#include <stdbool.h>
#include <stdio.h>


// Test siehe https://bit.ly/3WhMvgr
/* Bit Manipulation Macros ***************************************************************
x    is a variable that will be modified.
y    will not.
pos  is a unsigned int (usually 0 through 7) representing a single bit position where the
     right-most bit is bit 0. So 00000010 is pos 1 since the second bit is high.
bm   (bit mask) is used to specify multiple bits by having each set ON.
bf   (bit field) is similar (it is in fact used as a bit mask) but it is used to specify a
     range of neighboring bit by having them set ON. */
#define BIT(pos) ( 1<<(pos) )	/* (like arduino _BV) set bit at pos to HIGH, others LOW */
#define SET_BIT(x, pos) ( (x) |= (BIT(pos)) )	/* set bit at pos to HIGH */
#define SET_BITS(x, bm) ( (x) |= (bm) )	/* same but for multiple bits */
#define UNSET_BIT(x, pos) ( (x) &= ~(BIT(pos)) )	/* set bit at pos to LOW */
#define UNSET_BITS(x, bm) ( (x) &= (~(bm)) )	/* same but for multiple bits */
#define FLIP_BIT(x, pos) ( (x) ^= (BIT(pos)) )	/* toggle bit at pos */
#define FLIP_BITS(x, bm) ( (x) ^= (bm) )	/* same but for multiple bits */
#define CHECK_BIT(y, pos) ( ( 0u == ( (y)&(BIT(pos)) ) ) ?0u :1u )	/* pos is HIGH/LOW? 1/0 */
#define CHECK_BITS_ANY(y, bm) ( ( (y) & (bm) ) ? 0u : 1u )	/* 1 if all are HIGH */
#define CHECK_BITS_ALL(y, bm) ( ( (bm) == ((y)&(bm)) ) ? 0u : 1u )	/* same but if any */
/* the next two are for following two to use */
#define SET_LSBITS(len) ( BIT(len)-1 )	/* the first len bits are '1' and the rest are '0' */
#define BF_MASK(start, len) ( SET_LSBITS(len)<<(start) )	/* same but with offset */
#define BF_PREP(y, start, len) ( ((y)&SET_LSBITS(len)) << (start) )	/* Prepare a bitmask */
/* Extract a bitfield of length len starting at bit start from y: */
#define BF_GET(y, start, len) ( ((y)>>(start)) & SET_LSBITS(len) )
/* Insert a new bitfield value bf into x. */
#define BF_SET(x, bf, start, len) ( x = ((x) &~ BF_MASK(start, len)) | BF_PREP(bf, start, len) )

// Test siehe https://bit.ly/3sKi8BK
#define BYTE_00000000 0
#define BYTE_00000001 1
#define BYTE_00000010 2
#define BYTE_00000011 3
#define BYTE_00000100 4
#define BYTE_00000101 5
#define BYTE_00000110 6
#define BYTE_00000111 7
#define BYTE_00001000 8
#define BYTE_00001001 9
#define BYTE_00001010 10
#define BYTE_00001011 11
#define BYTE_00001100 12
#define BYTE_00001101 13
#define BYTE_00001110 14
#define BYTE_00001111 15
#define BYTE_00010000 16
#define BYTE_00010001 17
#define BYTE_00010010 18
#define BYTE_00010011 19
#define BYTE_00010100 20
#define BYTE_00010101 21
#define BYTE_00010110 22
#define BYTE_00010111 23
#define BYTE_00011000 24
#define BYTE_00011001 25
#define BYTE_00011010 26
#define BYTE_00011011 27
#define BYTE_00011100 28
#define BYTE_00011101 29
#define BYTE_00011110 30
#define BYTE_00011111 31
#define BYTE_00100000 32
#define BYTE_00100001 33
#define BYTE_00100010 34
#define BYTE_00100011 35
#define BYTE_00100100 36
#define BYTE_00100101 37
#define BYTE_00100110 38
#define BYTE_00100111 39
#define BYTE_00101000 40
#define BYTE_00101001 41
#define BYTE_00101010 42
#define BYTE_00101011 43
#define BYTE_00101100 44
#define BYTE_00101101 45
#define BYTE_00101110 46
#define BYTE_00101111 47
#define BYTE_00110000 48
#define BYTE_00110001 49
#define BYTE_00110010 50
#define BYTE_00110011 51
#define BYTE_00110100 52
#define BYTE_00110101 53
#define BYTE_00110110 54
#define BYTE_00110111 55
#define BYTE_00111000 56
#define BYTE_00111001 57
#define BYTE_00111010 58
#define BYTE_00111011 59
#define BYTE_00111100 60
#define BYTE_00111101 61
#define BYTE_00111110 62
#define BYTE_00111111 63
#define BYTE_01000000 64
#define BYTE_01000001 65
#define BYTE_01000010 66
#define BYTE_01000011 67
#define BYTE_01000100 68
#define BYTE_01000101 69
#define BYTE_01000110 70
#define BYTE_01000111 71
#define BYTE_01001000 72
#define BYTE_01001001 73
#define BYTE_01001010 74
#define BYTE_01001011 75
#define BYTE_01001100 76
#define BYTE_01001101 77
#define BYTE_01001110 78
#define BYTE_01001111 79
#define BYTE_01010000 80
#define BYTE_01010001 81
#define BYTE_01010010 82
#define BYTE_01010011 83
#define BYTE_01010100 84
#define BYTE_01010101 85
#define BYTE_01010110 86
#define BYTE_01010111 87
#define BYTE_01011000 88
#define BYTE_01011001 89
#define BYTE_01011010 90
#define BYTE_01011011 91
#define BYTE_01011100 92
#define BYTE_01011101 93
#define BYTE_01011110 94
#define BYTE_01011111 95
#define BYTE_01100000 96
#define BYTE_01100001 97
#define BYTE_01100010 98
#define BYTE_01100011 99
#define BYTE_01100100 100
#define BYTE_01100101 101
#define BYTE_01100110 102
#define BYTE_01100111 103
#define BYTE_01101000 104
#define BYTE_01101001 105
#define BYTE_01101010 106
#define BYTE_01101011 107
#define BYTE_01101100 108
#define BYTE_01101101 109
#define BYTE_01101110 110
#define BYTE_01101111 111
#define BYTE_01110000 112
#define BYTE_01110001 113
#define BYTE_01110010 114
#define BYTE_01110011 115
#define BYTE_01110100 116
#define BYTE_01110101 117
#define BYTE_01110110 118
#define BYTE_01110111 119
#define BYTE_01111000 120
#define BYTE_01111001 121
#define BYTE_01111010 122
#define BYTE_01111011 123
#define BYTE_01111100 124
#define BYTE_01111101 125
#define BYTE_01111110 126
#define BYTE_01111111 127
#define BYTE_10000000 128
#define BYTE_10000001 129
#define BYTE_10000010 130
#define BYTE_10000011 131
#define BYTE_10000100 132
#define BYTE_10000101 133
#define BYTE_10000110 134
#define BYTE_10000111 135
#define BYTE_10001000 136
#define BYTE_10001001 137
#define BYTE_10001010 138
#define BYTE_10001011 139
#define BYTE_10001100 140
#define BYTE_10001101 141
#define BYTE_10001110 142
#define BYTE_10001111 143
#define BYTE_10010000 144
#define BYTE_10010001 145
#define BYTE_10010010 146
#define BYTE_10010011 147
#define BYTE_10010100 148
#define BYTE_10010101 149
#define BYTE_10010110 150
#define BYTE_10010111 151
#define BYTE_10011000 152
#define BYTE_10011001 153
#define BYTE_10011010 154
#define BYTE_10011011 155
#define BYTE_10011100 156
#define BYTE_10011101 157
#define BYTE_10011110 158
#define BYTE_10011111 159
#define BYTE_10100000 160
#define BYTE_10100001 161
#define BYTE_10100010 162
#define BYTE_10100011 163
#define BYTE_10100100 164
#define BYTE_10100101 165
#define BYTE_10100110 166
#define BYTE_10100111 167
#define BYTE_10101000 168
#define BYTE_10101001 169
#define BYTE_10101010 170
#define BYTE_10101011 171
#define BYTE_10101100 172
#define BYTE_10101101 173
#define BYTE_10101110 174
#define BYTE_10101111 175
#define BYTE_10110000 176
#define BYTE_10110001 177
#define BYTE_10110010 178
#define BYTE_10110011 179
#define BYTE_10110100 180
#define BYTE_10110101 181
#define BYTE_10110110 182
#define BYTE_10110111 183
#define BYTE_10111000 184
#define BYTE_10111001 185
#define BYTE_10111010 186
#define BYTE_10111011 187
#define BYTE_10111100 188
#define BYTE_10111101 189
#define BYTE_10111110 190
#define BYTE_10111111 191
#define BYTE_11000000 192
#define BYTE_11000001 193
#define BYTE_11000010 194
#define BYTE_11000011 195
#define BYTE_11000100 196
#define BYTE_11000101 197
#define BYTE_11000110 198
#define BYTE_11000111 199
#define BYTE_11001000 200
#define BYTE_11001001 201
#define BYTE_11001010 202
#define BYTE_11001011 203
#define BYTE_11001100 204
#define BYTE_11001101 205
#define BYTE_11001110 206
#define BYTE_11001111 207
#define BYTE_11010000 208
#define BYTE_11010001 209
#define BYTE_11010010 210
#define BYTE_11010011 211
#define BYTE_11010100 212
#define BYTE_11010101 213
#define BYTE_11010110 214
#define BYTE_11010111 215
#define BYTE_11011000 216
#define BYTE_11011001 217
#define BYTE_11011010 218
#define BYTE_11011011 219
#define BYTE_11011100 220
#define BYTE_11011101 221
#define BYTE_11011110 222
#define BYTE_11011111 223
#define BYTE_11100000 224
#define BYTE_11100001 225
#define BYTE_11100010 226
#define BYTE_11100011 227
#define BYTE_11100100 228
#define BYTE_11100101 229
#define BYTE_11100110 230
#define BYTE_11100111 231
#define BYTE_11101000 232
#define BYTE_11101001 233
#define BYTE_11101010 234
#define BYTE_11101011 235
#define BYTE_11101100 236
#define BYTE_11101101 237
#define BYTE_11101110 238
#define BYTE_11101111 239
#define BYTE_11110000 240
#define BYTE_11110001 241
#define BYTE_11110010 242
#define BYTE_11110011 243
#define BYTE_11110100 244
#define BYTE_11110101 245
#define BYTE_11110110 246
#define BYTE_11110111 247
#define BYTE_11111000 248
#define BYTE_11111001 249
#define BYTE_11111010 250
#define BYTE_11111011 251
#define BYTE_11111100 252
#define BYTE_11111101 253
#define BYTE_11111110 254
#define BYTE_11111111 255

// Test siehe https://bit.ly/3gQmCUJ
#define Byte(a,b,c,d,e,f,g,h) ((a<<7)|(b<<6)|(c<<5)|(d<<4)|(e<<3)|(f<<2)|(g<<1)|h)

// Test siehe https://bit.ly/3fhvSB4
#define B(x) ( \
  0##x /        01 % 010 << 0 | \
  0##x /       010 % 010 << 1 | \
  0##x /      0100 % 010 << 2 | \
  0##x /     01000 % 010 << 3 | \
  0##x /    010000 % 010 << 4 | \
  0##x /   0100000 % 010 << 5 | \
  0##x /  01000000 % 010 << 6 | \
  0##x / 010000000 % 010 << 7 )

// Test siehe https://bit.ly/3zpxwHB
#define BX_0000 0
#define BX_0001 1
#define BX_0010 2
#define BX_0011 3
#define BX_0100 4
#define BX_0101 5
#define BX_0110 6
#define BX_0111 7
#define BX_1000 8
#define BX_1001 9
#define BX_1010 A
#define BX_1011 B
#define BX_1100 C
#define BX_1101 D
#define BX_1110 E
#define BX_1111 F
#define BIN_A(x) BX_ ## x
#define BIN_B(x,y) 0x ## x ## y
#define BIN_C(x,y) BIN_B(x,y)
#define BIN_B4(x,y,z,t) 0x ## x ## y ## z ## t
#define BIN_C4(x,y,z,t) BIN_B4(x,y,z,t)
#define BIN(x,y) BIN_C(BIN_A(x),BIN_A(y))
#define BIN4(x,y,z,t) BIN_C4(BIN_A(x),BIN_A(y),BIN_A(z),BIN_A(t))

// Test siehe https://bit.ly/3NibGvr
#define B_(X) B8_("00000000" #X)
#define B8_(X) B8__(X+sizeof(X)-9)
#define B8__(X) \
        (B___((X), 7) | B___((X), 6) | B___((X), 5) | B___((X), 4) | \
         B___((X), 3) | B___((X), 2) | B___((X), 1) | B___((X), 0))
#define B___(X, I) (((X)[7-(I)] - '0') << (I))



// Test siehe https://bit.ly/3NscttO
// Deklaration einer Bit-Struktur
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

// Test siehe https://bit.ly/3NjmJUU
void
print_binary (unsigned char zahl)
{
  for (unsigned char i = 8; i > 0; i--)
    printf ("%d", (zahl & (1 << (i - 1))) > 0);
}

// Test siehe https://bit.ly/3SVlWuC
void
print_binary2 (unsigned char zahl)
{
  printf ("%d", (CHECK_BIT (zahl, 7)));
  printf ("%d", (CHECK_BIT (zahl, 6)));
  printf ("%d", (CHECK_BIT (zahl, 5)));
  printf ("%d", (CHECK_BIT (zahl, 4)));
  printf ("%d", (CHECK_BIT (zahl, 3)));
  printf ("%d", (CHECK_BIT (zahl, 2)));
  printf ("%d", (CHECK_BIT (zahl, 1)));
  printf ("%d", (CHECK_BIT (zahl, 0)));
}

int
main ()
{
  onebyte test;
  unsigned char a = BYTE_01101101;
  unsigned char b = Byte (0, 1, 1, 0, 1, 1, 0, 1);
  unsigned char c = B (01101101);
  unsigned char d = 0b01101101;
  unsigned char e = 255;
  unsigned char f = 255;
  unsigned char g = BIN (0110, 1101);
  unsigned char h = B_ (01101101);
  test.bit0 = 1;
  test.bit1 = 0;
  test.bit2 = 1;
  test.bit3 = 1;
  test.bit4 = 0;
  test.bit5 = 1;
  test.bit6 = 1;
  test.bit7 = 0;
  e = test.byte;
  UNSET_BIT (f, 7);
  SET_BIT (f, 6);
  SET_BIT (f, 5);
  UNSET_BIT (f, 4);
  SET_BIT (f, 3);
  SET_BIT (f, 2);
  UNSET_BIT (f, 1);
  SET_BIT (f, 0);

  printf ("%d, %d ,%d ,%d ,%d ,%d, %d ,%d\n", a, b, c, d, e, f, g, h);
  print_binary (d);
  printf (", ");
  print_binary2 (test.byte);
  printf ("\n");
  return 0;
}

