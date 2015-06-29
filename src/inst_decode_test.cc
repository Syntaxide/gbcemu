#include "inst_decode.cc"

#include <stdio.h>
#include <iostream>

#define TEST_CATEGORY(name) puts(name)
#define TEST_EQ2(a, b, c) {                        \
  auto t1 = a;\
  auto t2 = b;\
  if(t1 == t2){\
    printf("OK\n");\
  }else{\
    printf("FAIL\t%d != %d at %d " #b "\n", t1, t2, c);  \
  }\
  }
#define TEST_EQ(a, b) TEST_EQ2(a,b, __LINE__)

void test_select() {
  TEST_CATEGORY("instruction decoding");
  TEST_EQ(select_bits(0x1, 0, 0), 1);
  TEST_EQ(select_bits(0xf2, 1, 0), 2);
  TEST_EQ(select_bits(0xf2, 7, 7), 1);
  TEST_EQ(select_bits(0xf2, 7, 6), 3);
}

void check_instr(uint8_t rom1, Instruction::Operation op, char bytes_expected, int line) {
  Instruction instruction;
  uint8_t rom[3];
  rom[0] = rom1;
  rom[1] = 123;
  rom[2] = 213;
  printf("checking operation %s\n", OperationStrings[op]);
  decode_instruction(rom, &instruction);
  TEST_EQ2(instruction.operation, op, line);
  TEST_EQ2(instruction.bytes_used, bytes_expected, line);
}

void test_instr_decode() {
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
  rom[1] = 238;
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
  TEST_EQ(instruction.op2, 1);
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
  TEST_EQ(instruction.immediate, 128);
  TEST_EQ(instruction.bytes_used, 2);

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
  check_instr(0b00100010,
              Instruction::OP_LDHLIA,
              1,
              __LINE__);
  // LD  (HLD), A
  check_instr(0b00110010,
              Instruction::OP_LDHLDA,
              1,
              __LINE__);
  // LD dd, nn
  check_instr(0b00000001,
              Instruction::OP_LDDDNN,
              3,
              __LINE__);
  // LD SP, HL
  check_instr(0b11111001,
              Instruction::OP_LDSPHL,
              1,
              __LINE__);
  // PUSH qq
  check_instr(0b11000101,
              Instruction::OP_PUSHQQ,
              1,
              __LINE__);
  // POP qq
  check_instr(0b11000001,
              Instruction::OP_POPQQ,
              1,
              __LINE__);
  // LDHL SP, e
  check_instr(0b11111000,
              Instruction::OP_LDHLSP,
              2,
              __LINE__);
  // LD nn, SP
  check_instr(0b00001000,
              Instruction::OP_LDNNSP,
              3,
              __LINE__);
  // ADD A, R
  check_instr(0b10000000,
              Instruction::OP_ADDAR,
              1,
              __LINE__);
  // ADD A, N
  check_instr(0b11000110,
              Instruction::OP_ADDAN,
              2,
              __LINE__);
  // ADD A, HL
  check_instr(0b10000110,
              Instruction::OP_ADDAHL,
              1,
              __LINE__);
  // ADC A, R
  check_instr(0b10001001,
              Instruction::OP_ADCAR,
              1,
              __LINE__);
  // ADC A, N
  check_instr(0b11001110,
              Instruction::OP_ADCAN,
              2,
              __LINE__);
  // ADC A, HL
  check_instr(0b10001110,
              Instruction::OP_ADCAHL,
              1,
              __LINE__);
  // SUB R
  check_instr(0b10010001,
              Instruction::OP_SUBR,
              1,
              __LINE__);
  // SUB N
  check_instr(0b11010110,
              Instruction::OP_SUBN,
              2,
              __LINE__);
  // SUB HL
  check_instr(0b10010110,
              Instruction::OP_SUBHL,
              1,
              __LINE__);
  // SBC A, R
  check_instr(0b10011001,
              Instruction::OP_SBCAR,
              1,
              __LINE__);
  // SBC A, N
  check_instr(0b11011110,
              Instruction::OP_SBCAN,
              2,
              __LINE__);
  // SBC A, HL
  check_instr(0b10011110,
              Instruction::OP_SBCAHL,
              1,
              __LINE__);
  //AND R
  check_instr(0b10100001,
              Instruction::OP_ANDR,
              1,
              __LINE__);
  // AND N
  check_instr(0b11100110,
              Instruction::OP_ANDN,
              2,
              __LINE__);
  // AND HL
  check_instr(0b10100110,
              Instruction::OP_ANDHL,
              1,
              __LINE__);
  // OR R
  check_instr(0b10110001,
              Instruction::OP_ORR,
              1,
              __LINE__);
  // OR N
  check_instr(0b11110110,
              Instruction::OP_ORN,
              2,
              __LINE__);
  // OR HL
  check_instr(0b10110110,
              Instruction::OP_ORHL,
              1,
              __LINE__);
  // XOR R
  check_instr(0b10101001,
              Instruction::OP_XORR,
              1,
              __LINE__);
  // XOR N
  check_instr(0b11101110,
              Instruction::OP_XORN,
              2,
              __LINE__);
  // XOR HL
  check_instr(0b10101110,
              Instruction::OP_XORHL,
              1,
              __LINE__);
  // CP R
  check_instr(0b10111001,
              Instruction::OP_CPR,
              1,
              __LINE__);
  // CP N
  check_instr(0b11111110,
              Instruction::OP_CPN,
              2,
              __LINE__);
  // CP HL
  check_instr(0b10111110,
              Instruction::OP_CPHL,
              1,
              __LINE__);
  // INC R
  check_instr(0b00001100,
              Instruction::OP_INCR,
              1,
              __LINE__);
  // INC HL
  check_instr(0b00110100,
              Instruction::OP_INCHL,
              1,
              __LINE__);
  // DEC R
  check_instr(0b00001101,
              Instruction::OP_DECR,
              1,
              __LINE__);
  // DEC HL
  check_instr(0b00110101,
              Instruction::OP_DECHL,
              1,
              __LINE__);
}

int main(void) {
  test_select();
  test_instr_decode();
  return 0;
}
