#include "inst_decode.cc"

#include <stdio.h>
#include <iostream>

#define TEST_CATEGORY(name) puts(name)
#define TEST_EQ(a, b) {\
  auto t1 = a;\
  auto t2 = b;\
  printf(#a" = " #b "\t");                  \
  if(t1 == t2){\
    puts("success");\
  }else{\
  printf("%d != %d\n", t1, t2);\
  }\
  }

int main(void) {
    TEST_CATEGORY("instruction decoding");
    TEST_EQ(select_bits(0x1, 0, 0), 1);
    TEST_EQ(select_bits(0xf2, 1, 0), 2);
    TEST_EQ(select_bits(0xf2, 7, 7), 1);
    TEST_EQ(select_bits(0xf2, 7, 6), 3);

    unsigned char instr[] = {0b01111001, // LD A, C
                             0b00111110, // LD A, N
    };
    for(int i=0;i<2;i++)
      decode_instruction(instr+i, false);

    return 0;
}
