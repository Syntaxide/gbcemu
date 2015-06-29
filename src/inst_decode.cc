#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
enum reg_encoding {
  REG_b = 0,
  REG_c = 1,
  REG_2 = 2,
  REG_e = 3,
  REG_h = 4,
  REG_d = 5,
  REG_6 = 6,
  REG_a = 7
};

const uint8_t MASK_76 = 0xC0;
const uint8_t MASK_543 = 0x38;
const uint8_t MASK_210 = 0x07;

// incides   76543210
// select(0xb10101010)
uint8_t select_bits(uint8_t in, char start, char end) {
  uint8_t ones = ((1 << (start-end+1)) - 1) << end;
  return (in & ones) >> end;
}

struct Instruction {
  enum Operation {
    // 8 bit transfer / IO (p85)
    OP_LDRR = 1,
    OP_LDRN,
    OP_LDRHL,
    OP_LDHLR,
    OP_LDHLN,
    OP_LDABC,
    OP_LDADE,
    OP_LDAC,
    OP_LDCA,
    OP_LDAN,
    OP_LDNA,
    OP_LDANN,
    OP_LDNNA,
    OP_LDAHLI,
    OP_LDAHLD,
    OP_LDBCA,
    OP_LDDEA,
    OP_LDHLIA,
    OP_LDHLDA,

    // 16 bit transfer (p90)
    OP_LDDDNN,
    OP_LDSPHL,
    OP_PUSHQQ,
    OP_POPQQ,
    OP_LDHLSP,
    OP_LDNNSP,

    // 8bit arith, logical (p92)
    OP_ADDAR,
    OP_ADDAN,
    OP_ADDAHL,

    OP_ADCAR,
    OP_ADCAN,
    OP_ADCAHL,

    OP_SUBR,
    OP_SUBN,
    OP_SUBHL,

    OP_SBCAR,

    OP_SBCAN,
    OP_SBCAHL,

    OP_ANDR,
    OP_ANDN,
    OP_ANDHL,

    OP_ORR,
    OP_ORN,
    OP_ORHL,

    OP_XORR,
    OP_XORN,
    OP_XORHL,

    OP_CPR,
    OP_CPN,
    OP_CPHL,

    OP_INCR,
    OP_INCHL,

    OP_DECR,
    OP_DECHL,

  } operation;
  uint8_t op1, op2;
  uint8_t immediate;
  uint8_t immediate2;
  uint8_t bytes_used;
};


bool is_reg(uint8_t code) {
  return (code < 8) && (code != 2) && (code != 6);
}

#define INSTR_REGISTER_DEF(binary, label, len)\
  else if(*rom == binary) {                           \
    decoded->operation = label;                       \
    decoded->bytes_used = len;                        \
  }
#define INSTR_REGISTER_REG(first, two, label, len)      \
  else if(instr == first && op2 == two && is_reg(op1)){               \
  decoded->operation = label;                       \
  decoded->bytes_used = len;                        \
  }
#define INSTR_REGISTER_REG2(first, one, label, len)                      \
  else if(instr == first && op1 == one && is_reg(op2)){                 \
    decoded->operation = label; \
    decoded->bytes_used = len; \
    }


void decode_instruction(unsigned char *rom, Instruction *decoded) {
  memset(decoded, 0, sizeof(Instruction));
  uint8_t instr = select_bits(*rom, 7, 6);
  uint8_t op1 = decoded->op1 = select_bits(*rom, 5, 3);
  uint8_t op2 = decoded->op2 = select_bits(*rom, 2, 0);
  decoded->immediate = rom[1];
  decoded->immediate2 = rom[2];
  decoded->bytes_used = 1;
  printf("i=%d\top1=%d\top2=%d\n", instr, decoded->op1, decoded->op2);
    if(*rom == 0b00110110) {
      decoded->operation = Instruction::OP_LDHLN;
      decoded->bytes_used = 2;
    } else if(instr == 0 &&op2 == REG_6 && is_reg(op1)){
      decoded->operation = Instruction::OP_LDRN;
      decoded->bytes_used = 2;
    } else if(*rom == 0b00001010) {
      decoded->operation = Instruction::OP_LDABC;
    } else if(*rom == 0b00011010) {
      decoded->operation = Instruction::OP_LDADE;
    } else if(*rom == 0b00101010) {
      decoded->operation = Instruction::OP_LDAHLI;
    } else if(*rom == 0b00111010) {
      decoded->operation = Instruction::OP_LDAHLD;
    } else if(*rom == 0b00000010) {
      decoded->operation = Instruction::OP_LDBCA;
    } else if(*rom == 0b00010010) {
      decoded->operation = Instruction::OP_LDDEA;
    } else if(*rom == 0b00100010) {
      decoded->operation = Instruction::OP_LDHLIA;
    } else if(*rom == 0b00110010) {
      decoded->operation = Instruction::OP_LDHLDA;
    } else if((*rom & 0b11001111) == 1) {
      decoded->operation = Instruction::OP_LDDDNN;
      decoded->bytes_used = 3;
    } else if(*rom == 0b00001000) {
      decoded->operation = Instruction::OP_LDNNSP;
      decoded->bytes_used = 3;
    } else if(instr == 0 && op2 == 4) {
      decoded->operation = Instruction::OP_INCR;
    } else if(*rom == 0b00110100) {
      decoded->operation = Instruction::OP_INCHL;
    } else if(instr == 0 && op2 == 5) {
      decoded->operation = Instruction::OP_DECR;
    } else if(*rom == 0b00110101) {
      decoded->operation = Instruction::OP_DECHL;
    } else if(instr == 1 && op1 == REG_6 && is_reg(op2)) {
      decoded->operation = Instruction::OP_LDHLR;
    } else if(instr == 1 && op2 == REG_6 && is_reg(op1)) {
      decoded->operation = Instruction::OP_LDRHL;
    } else if(instr == 1 && is_reg(op1) && is_reg(op2)){
      decoded->operation = Instruction::OP_LDRR;
    } else if(instr == 2 && op1 == 2 && is_reg(op2)) {
      decoded->operation = Instruction::OP_SUBR;
    } else if(instr == 2 && op1 == 3 && is_reg(op2)) {
      decoded->operation = Instruction::OP_SBCAR;
    } else if(*rom == 0b10011110) {
      decoded->operation = Instruction::OP_SBCAHL;
    } else if(*rom == 0b10100110) {
      decoded->operation = Instruction::OP_ANDHL;
    } else if(instr == 2 && op1 == 6 && is_reg(op2)) {
      decoded->operation = Instruction::OP_ORR;
    } else if(*rom == 0b10110110) {
      decoded->operation = Instruction::OP_ORHL;
    } else if(instr == 2 && op1 == 5 && is_reg(op2)) {
      decoded->operation = Instruction::OP_XORR;
    } else if(*rom == 0b10101110) {
      decoded->operation = Instruction::OP_XORHL;
    } else if(instr == 2 && op1 == 7 && is_reg(op2)) {
      decoded->operation = Instruction::OP_CPR;
    } else if(*rom == 0b10111110) {
      decoded->operation = Instruction::OP_CPHL;
    } else if(instr == 2 && op1 == 0 && is_reg(op2)) {
      decoded->operation = Instruction::OP_ADDAR;
    } else if(instr == 2 && *rom == 0b10000110) {
      decoded->operation = Instruction::OP_ADDAHL;
    } else if(instr == 2 && op1 == 1 && is_reg(op2)) {
      decoded->operation = Instruction::OP_ADCAR;
    } else if(*rom == 0b10001110) {
      decoded->operation = Instruction::OP_ADCAHL;
    } else if(*rom == 0b11110010) {
      decoded->operation = Instruction::OP_LDAC;
    } else if(*rom == 0b11100010) {
      decoded->operation = Instruction::OP_LDCA;
    } else if(*rom == 0b11110000) {
      decoded->operation = Instruction::OP_LDAN;
      decoded->bytes_used = 2;
    } else if(*rom == 0b11100000) {
      decoded->operation = Instruction::OP_LDNA;
      decoded->bytes_used = 2;
    } else if(*rom == 0b11111010) {
      decoded->operation = Instruction::OP_LDANN;
      decoded->bytes_used = 3;
    } else if(*rom == 0b11101010) {
      decoded->operation = Instruction::OP_LDNNA;
      decoded->bytes_used = 3;
    } else if(*rom == 0b11111001) {
      decoded->operation = Instruction::OP_LDSPHL;
    } else if((*rom & 0b11001111) == 0b11000101) {
      decoded->operation = Instruction::OP_PUSHQQ;
    } else if((*rom & 0b11001111) == 0b11000001) {
      decoded->operation = Instruction::OP_POPQQ;
    } else if(*rom == 0b11111000) {
      decoded->operation = Instruction::OP_LDHLSP;
      decoded->bytes_used = 2;
    } else if(*rom == 0b11010110) {
      decoded->operation = Instruction::OP_SUBN;
      decoded->bytes_used = 2;
    } else if(*rom == 0b10010110) {
      decoded->operation = Instruction::OP_SUBHL;
      decoded->bytes_used = 1;
    } else if(*rom == 0b11011110) {
      decoded->operation = Instruction::OP_SBCAN;
      decoded->bytes_used = 2;
    } else if(*rom == 0b11100110) {
      decoded->operation = Instruction::OP_ANDN;
      decoded->bytes_used = 2;
    } else if(*rom == 0b11110110) {
      decoded->operation = Instruction::OP_ORN;
      decoded->bytes_used = 2;
    } else if(*rom == 0b11101110) {
      decoded->operation = Instruction::OP_XORN;
      decoded->bytes_used = 2;
    } else if(*rom == 0b11111110) {
      decoded->operation = Instruction::OP_CPN;
      decoded->bytes_used = 2;
    } else if(*rom == 0b11000110) {
      decoded->operation = Instruction::OP_ADDAN;
    } else if(*rom == 0b11001110) {
      decoded->operation = Instruction::OP_ADCAN;
      decoded->bytes_used = 2;
    }
    INSTR_REGISTER_REG2(2, 0b10, Instruction::OP_SUBR, 1)
    INSTR_REGISTER_DEF(0b11010110, Instruction::OP_SUBN, 2)
    INSTR_REGISTER_DEF(0b10010110, Instruction::OP_SUBHL, 1)

    INSTR_REGISTER_REG2(2, 0b011, Instruction::OP_SBCAR, 1)
    INSTR_REGISTER_DEF(0b11011110, Instruction::OP_SBCAN, 2)
    INSTR_REGISTER_DEF(0b10011110, Instruction::OP_SBCAHL, 1)

    INSTR_REGISTER_REG2(2, 0b100, Instruction::OP_ANDR, 1)
    INSTR_REGISTER_DEF(0b11100110, Instruction::OP_ANDN, 2)
    INSTR_REGISTER_DEF(0b10100110, Instruction::OP_ANDHL, 1)

    INSTR_REGISTER_REG2(2, 0b110, Instruction::OP_ORR, 1)
    INSTR_REGISTER_DEF(0b11110110, Instruction::OP_ORN, 2)
    INSTR_REGISTER_DEF(0b10110110, Instruction::OP_ORHL, 1)

    INSTR_REGISTER_REG2(2, 0b101, Instruction::OP_XORR, 1)
    INSTR_REGISTER_DEF(0b11101110, Instruction::OP_XORN, 2)
    INSTR_REGISTER_DEF(0b10101110, Instruction::OP_XORHL, 1)

    INSTR_REGISTER_REG2(2, 0b111, Instruction::OP_CPR, 1)
    INSTR_REGISTER_DEF(0b11111110, Instruction::OP_CPN, 2)
    INSTR_REGISTER_DEF(0b10111110, Instruction::OP_CPHL, 1)

    INSTR_REGISTER_REG(0, 0b100, Instruction::OP_INCR, 1)
    INSTR_REGISTER_DEF(0b00110100, Instruction::OP_INCHL, 1)

    INSTR_REGISTER_REG(0, 0b101, Instruction::OP_DECR, 1)
    INSTR_REGISTER_DEF(0b00110101, Instruction::OP_DECHL, 1)
}
