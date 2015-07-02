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
  TEST_EQ(select_bits(0b11, 1, 0), 3);
}

#define CHECK_INSTR(rom1, op, bytes_expected) check_instr(rom1, op, bytes_expected, __LINE__);
#define CHECK_INSTR2(rom1, rom2, op, bytes_expected) check_instr2(rom1, rom2, op, bytes_expected, __LINE__);

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

void check_instr2(uint8_t rom1, uint8_t rom2, Instruction::Operation op, char bytes_expected, int line) {
  Instruction instruction;
  uint8_t rom[3];
  rom[0] = rom1;
  rom[1] = rom2;
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
  CHECK_INSTR(0b00100010,
              Instruction::OP_LDHLIA,
              1);
  // LD  (HLD), A
  CHECK_INSTR(0b00110010,
              Instruction::OP_LDHLDA,
              1);
  // LD dd, nn
  CHECK_INSTR(0b00000001,
              Instruction::OP_LDDDNN,
              3);
  // LD SP, HL
  CHECK_INSTR(0b11111001,
              Instruction::OP_LDSPHL,
              1);
  // PUSH qq
  CHECK_INSTR(0b11000101,
              Instruction::OP_PUSHQQ,
              1);
  // POP qq
  CHECK_INSTR(0b11000001,
              Instruction::OP_POPQQ,
              1);
  // LDHL SP, e
  CHECK_INSTR(0b11111000,
              Instruction::OP_LDHLSP,
              2);
  // LD nn, SP
  CHECK_INSTR(0b00001000,
              Instruction::OP_LDNNSP,
              3);
  // ADD A, R
  CHECK_INSTR(0b10000000,
              Instruction::OP_ADDAR,
              1);
  // ADD A, N
  CHECK_INSTR(0b11000110,
              Instruction::OP_ADDAN,
              2);
  // ADD A, HL
  CHECK_INSTR(0b10000110,
              Instruction::OP_ADDAHL,
              1);
  // ADC A, R
  CHECK_INSTR(0b10001001,
              Instruction::OP_ADCAR,
              1);
  // ADC A, N
  CHECK_INSTR(0b11001110,
              Instruction::OP_ADCAN,
              2);
  // ADC A, HL
  CHECK_INSTR(0b10001110,
              Instruction::OP_ADCAHL,
              1);
  // SUB R
  CHECK_INSTR(0b10010001,
              Instruction::OP_SUBR,
              1);
  // SUB N
  CHECK_INSTR(0b11010110,
              Instruction::OP_SUBN,
              2);
  // SUB HL
  CHECK_INSTR(0b10010110,
              Instruction::OP_SUBHL,
              1);
  // SBC A, R
  CHECK_INSTR(0b10011001,
              Instruction::OP_SBCAR,
              1);
  // SBC A, N
  CHECK_INSTR(0b11011110,
              Instruction::OP_SBCAN,
              2);
  // SBC A, HL
  CHECK_INSTR(0b10011110,
              Instruction::OP_SBCAHL,
              1);
  //AND R
  CHECK_INSTR(0b10100001,
              Instruction::OP_ANDR,
              1);
  // AND N
  CHECK_INSTR(0b11100110,
              Instruction::OP_ANDN,
              2);
  // AND HL
  CHECK_INSTR(0b10100110,
              Instruction::OP_ANDHL,
              1);
  // OR R
  CHECK_INSTR(0b10110001,
              Instruction::OP_ORR,
              1);
  // OR N
  CHECK_INSTR(0b11110110,
              Instruction::OP_ORN,
              2);
  // OR HL
  CHECK_INSTR(0b10110110,
              Instruction::OP_ORHL,
              1);
  // XOR R
  CHECK_INSTR(0b10101001,
              Instruction::OP_XORR,
              1);
  // XOR N
  CHECK_INSTR(0b11101110,
              Instruction::OP_XORN,
              2);
  // XOR HL
  CHECK_INSTR(0b10101110,
              Instruction::OP_XORHL,
              1);
  // CP R
  CHECK_INSTR(0b10111001,
              Instruction::OP_CPR,
              1);
  // CP N
  CHECK_INSTR(0b11111110,
              Instruction::OP_CPN,
              2);
  // CP HL
  CHECK_INSTR(0b10111110,
              Instruction::OP_CPHL,
              1);
  // INC R
  CHECK_INSTR(0b00001100,
              Instruction::OP_INCR,
              1);
  // INC HL
  CHECK_INSTR(0b00110100,
              Instruction::OP_INCHL,
              1);
  // DEC R
  CHECK_INSTR(0b00001101,
              Instruction::OP_DECR,
              1);
  // DEC HL
  CHECK_INSTR(0b00110101,
              Instruction::OP_DECHL,
              1);

  // ADD HL, SS
  CHECK_INSTR(0b00111001,
              Instruction::OP_ADDHLSS,
              1);

  // ADD SP, e
  CHECK_INSTR(0b11101000,
              Instruction::OP_ADDSPE,
              2);

  // INC SS
  CHECK_INSTR(0b00000011,
              Instruction::OP_INCSS,
              1);
  // DEC SS
  CHECK_INSTR(0b00001011,
              Instruction::OP_DECSS,
              1);
  // RCLA
  CHECK_INSTR(0b00000111,
              Instruction::OP_RLCA,
              1);
  // RLA
  CHECK_INSTR(0b00010111,
              Instruction::OP_RLA,
              1);
  // RRCA
  CHECK_INSTR(0b00001111,
              Instruction::OP_RRCA,
              1);
  // RRA
  CHECK_INSTR(0b00011111,
              Instruction::OP_RRA,
              1);
  // RLCR
  CHECK_INSTR2(0b11001011,
               0b00000001,
               Instruction::OP_RLCR,
               2); // RLCHL
  CHECK_INSTR2(0b11001011,
               0b00000110,
               Instruction::OP_RLCHL,
               2);
  CHECK_INSTR2(0b11001011,
               0b00010001,
               Instruction::OP_RLR,
               2);
  CHECK_INSTR2(0b11001011,
               0b00010110,
               Instruction::OP_RLHL,
               2);
  CHECK_INSTR2(0b11001011, 
	       0b00001001,
	       Instruction::OP_RRCR,
	       2);
  CHECK_INSTR2(0b11001011,
	       0b00001110,
	       Instruction::OP_RRCHL,
	       2);
  CHECK_INSTR2(0b11001011,
	       0b00011001,
	       Instruction::OP_RRR,
	       2);
  CHECK_INSTR2(0b11011011,
	       0b00011110,
	       Instruction::OP_RRHL,
	       2);
  CHECK_INSTR2(0b11001011,
	       0b00100001,
	       Instruction::OP_SLAR,
	       2);
  CHECK_INSTR2(0b11011011,
	       0b00100110,
	       Instruction::OP_SLAHL,
	       2);
  CHECK_INSTR2(0b11001011,
	       0b00101001,
	       Instruction::OP_SRAR,
	       2);
  CHECK_INSTR2(0b11001011,
	       0b00101110,
	       Instruction::OP_SRARHL,
	       2);
  CHECK_INSTR2(0b11001011,
	       0b00111001,
	       Instruction::OP_SRLR,
	       2);
  CHECK_INSTR2(0b11001011,
	       0b00111110,
	       Instruction::OP_SRLHL,
	       2);
  CHECK_INSTR2(0b11001011,
	       0b00110001,
	       Instruction::OP_SWAPR,
	       2);
  CHECK_INSTR2(0b11001011,
	       0b00110110,
	       Instruction::OP_SWAPHL,
	       2);
  CHECK_INSTR2(0b11001011,
	       0b01111001,
	       Instruction::OP_BITR,
	       2);
  CHECK_INSTR2(0b11001011,
	       0b01111110,
	       Instruction::OP_BITHL,
	       2);
  CHECK_INSTR2(0b11001011,
	       0b11111001,
	       Instruction::OP_SETR,
	       2);
  CHECK_INSTR2(0b11001011,
	       0b11111110,
	       Instruction::OP_SETHL,
	       2);
  CHECK_INSTR2(0b11001011,
	       0b10111001,
	       Instruction::OP_RESR,
	       2);
  CHECK_INSTR2(0b11001011,
	       0b10111110,
	       Instruction::OP_RESHL,
	       2);
  CHECK_INSTR(0b11000011,
	      Instruction::OP_JPNN,
	      3);
  CHECK_INSTR(0b11000010,
	      Instruction::OP_JPCC,
	      3);
  CHECK_INSTR(0b00011000,
	      Instruction::OP_JRE,
	      2);
  CHECK_INSTR(0b00111000,
	      Instruction::OP_JRCC,
	      2);
  CHECK_INSTR(0b11101001,
	      Instruction::OP_JPHL,
	      1);
  CHECK_INSTR(0b11001101,
	      Instruction::OP_CALLNN,
	      3);
  CHECK_INSTR(0b11000100,
	      Instruction::OP_CALLCC,
	      3);
  CHECK_INSTR(0b11001001,
	      Instruction::OP_RET,
	      1);
  CHECK_INSTR(0b11011001,
	      Instruction::OP_RETI,
	      1);
  CHECK_INSTR(0b11000000,
	      Instruction::OP_RETCC,
	      1);
  CHECK_INSTR(0b11111111,
	      Instruction::OP_RST,
	      1);
  CHECK_INSTR(0b00100111,
	      Instruction::OP_DAA,
	      1);
  CHECK_INSTR(0b00101111,
	      Instruction::OP_CPL,
	      1);
  CHECK_INSTR(0b00000000,
	      Instruction::OP_NOP,
	      1);
  CHECK_INSTR(0b01110110,
	      Instruction::OP_HALT,
	      1);
  CHECK_INSTR2(0b00010000,
	       0b00000000,
	       Instruction::OP_STOP,
	       2);



		
	
		

}

int main(void) {
  test_select();
  test_instr_decode();
  return 0;
}
