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
    printf("\t\tFAIL\t%d != %d at %d\n", t1, t2, __LINE__);  \
  }\
  }

void test_select() {
  TEST_CATEGORY("instruction decoding");
  TEST_EQ(select_bits(0x1, 0, 0), 1);
  TEST_EQ(select_bits(0xf2, 1, 0), 2);
  TEST_EQ(select_bits(0xf2, 7, 7), 1);
  TEST_EQ(select_bits(0xf2, 7, 6), 3);
}

void test_ld_decode() {
  TEST_CATEGORY("testing ld instructions");
  Instruction instruction;
  unsigned char rom[2] = {0, 0};

  // LD R(a) R(B)
  rom[0] = 0b01111000;
  decode_instruction(rom, &instruction);
  TEST_EQ(instruction.operation, Instruction::OP_LDRR);
  TEST_EQ(instruction.op1, 3);
  TEST_EQ(instruction.op2, 0);

  // LD R(a), N
  rom[0] = 0b00111110;
  rom[2] = 238;
  decode_instruction(rom, &instruction);
  TEST_EQ(instruction.operation, Instruction::OP_LDRN);
  TEST_EQ(instruction.op1, 3);
  TEST_EQ(instruction.immediate, 238);

  // LD R(B), (HL)
  rom[0] = 0b01001110;
  decode_instruction(rom, &instruction);
  TEST_EQ(instruction.operation, Instruction::OP_LDRHL);
  TEST_EQ(instruction.op1, 1);

  // LD (HL), R
  rom[0] = 0b01110001;
  decode_instruction(rom, &instruction);
  TEST_EQ(instruction.operation, Instruction::OP_LDHLR);
  TEST_EQ(instruction.op1, 1);

  // LD (HL), N(238)
  rom[0] = 0b00110110;
  rom[1] = 238;
  decode_instruction(rom, &instruction);
  TEST_EQ(instruction.operation, Instruction::OP_LDHLN);
  TEST_EQ(instruction.immediate, 238);

  // LD A, BC
  rom[0] = 0b00001010;
  decode_instruction(rom, &instruction);
  TEST_EQ(instruction.operation, Instruction::OP_LDABC);

  // LD A, DE
  rom[0] = 0b00011010;
  decode_instruction(rom, &instruction);
  TEST_EQ(instruction.operation, Instruction::OP_LDADE);

  // LD A, (C)
  rom[0] = 0b11110010;
  decode_instruction(rom, &instruction);
  TEST_EQ(instruction.operation, Instruction::OP_LDAC);

}

int main(void) {
  test_select();
  test_ld_decode();
  return 0;
}
