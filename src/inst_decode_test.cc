#include "inst_decode.cc"

#include <stdio.h>
#include <iostream>

#define TEST_CATEGORY(name) puts(name)
#define TEST_EQ(a, b) {\
  auto t1 = a;\
  auto t2 = b;\
  if(t1 == t2){\
    printf("OK\t");\
  }else{\
    printf("FAIL\t%d != %d at %d\t", t1, t2, __LINE__);  \
  }\
  printf(#a" = " #b "\t\n");                      \
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
  TEST_EQ(instruction.op1, 7);
  TEST_EQ(instruction.op2, 0);
  TEST_EQ(instruction.bytes_used, 1);

  // LD R(a), N
  rom[0] = 0b00111110;
  rom[2] = 238;
  decode_instruction(rom, &instruction);
  TEST_EQ(instruction.operation, Instruction::OP_LDRN);
  TEST_EQ(instruction.op1, 7);
  TEST_EQ(instruction.immediate, 238);
  TEST_EQ(instruction.bytes_used, 2);

  // LD R(B), (HL)
  rom[0] = 0b01001110;
  decode_instruction(rom, &instruction);
  TEST_EQ(instruction.operation, Instruction::OP_LDRHL);
  TEST_EQ(instruction.op1, 1);
  TEST_EQ(instruction.bytes_used, 1);

  // LD (HL), R
  rom[0] = 0b01110001;
  decode_instruction(rom, &instruction);
  TEST_EQ(instruction.operation, Instruction::OP_LDHLR);
  TEST_EQ(instruction.op1, 1);
  TEST_EQ(instruction.bytes_used, 1);

  // LD (HL), N(238)
  rom[0] = 0b00110110;
  rom[1] = 238;
  decode_instruction(rom, &instruction);
  TEST_EQ(instruction.operation, Instruction::OP_LDHLN);
  TEST_EQ(instruction.immediate, 238);
  TEST_EQ(instruction.bytes_used, 2);

  // LD A, BC
  rom[0] = 0b00001010;
  decode_instruction(rom, &instruction);
  TEST_EQ(instruction.operation, Instruction::OP_LDABC);
  TEST_EQ(instruction.bytes_used, 1);

  // LD A, DE
  rom[0] = 0b00011010;
  decode_instruction(rom, &instruction);
  TEST_EQ(instruction.operation, Instruction::OP_LDADE);
  TEST_EQ(instruction.bytes_used, 1);

  // LD A, (C)
  rom[0] = 0b11110010;
  decode_instruction(rom, &instruction);
  TEST_EQ(instruction.operation, Instruction::OP_LDAC);
  TEST_EQ(instruction.bytes_used, 1);

  // LD (C), A
  rom[0] = 0b11100010;
  decode_instruction(rom, &instruction);
  TEST_EQ(instruction.operation, Instruction::OP_LDCA);
  TEST_EQ(instruction.bytes_used, 1);

  // LD A, (N)
  rom[0] = 0b11110000;
  rom[1] = 128;
  decode_instruction(rom, &instruction);
  TEST_EQ(instruction.operation, Instruction::OP_LDAN);
  TEST_EQ(instruction.immediate, 138);
  TEST_EQ(instruction.bytes_used, 1);

  // LD (N), A
  rom[0] = 0b11100000;
  rom[1] = 210;
  decode_instruction(rom, &instruction);
  TEST_EQ(instruction.operation, Instruction::OP_LDNA);
  TEST_EQ(instruction.immediate, 210);
  TEST_EQ(instruction.bytes_used, 2);

  // LD A, (NN)
  rom[0] = 0b11111010;
  rom[1] = 210;
  rom[2] = 56;
  decode_instruction(rom, &instruction);
  TEST_EQ(instruction.operation, Instruction::OP_LDANN);
  TEST_EQ(instruction.immediate, 210);
  TEST_EQ(instruction.immediate2, 56);
  TEST_EQ(instruction.bytes_used, 3);

  // LD (NN), A
  rom[0] = 0b11101010;
  rom[1] = 123;
  rom[2] = 231;
  decode_instruction(rom, &instruction);
  TEST_EQ(instruction.operation, Instruction::OP_LDNNA);
  TEST_EQ(instruction.immediate, 123);
  TEST_EQ(instruction.immediate2, 231);
  TEST_EQ(instruction.bytes_used, 3);

  // LD (HLI), A
  rom[0] = 0b00100010;
  decode_instruction(rom, &instruction);
  TEST_EQ(instruction.operation, Instruction::OP_LDHLIA);
  TEST_EQ(instruction.bytes_used, 1);

  // LD  (HLD), A
  rom[0] = 0b00110010;
  decode_instruction(rom, &instruction);
  TEST_EQ(instruction.operation, Instruction::OP_LDHLDA);
  TEST_EQ(instruction.bytes_used, 1)

}

int main(void) {
  test_select();
  test_ld_decode();
  return 0;
}
