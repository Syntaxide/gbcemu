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

    // 8bit arith, logical
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
    OP_SBCHL,

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
void decode_instruction(unsigned char *rom, Instruction *decoded) {
  memset(decoded, 0, sizeof(Instruction));
  uint8_t instr = select_bits(*rom, 7, 6);
  uint8_t op1 = decoded->op1 = select_bits(*rom, 5, 3);
  uint8_t op2 = decoded->op2 = select_bits(*rom, 2, 0);
  decoded->immediate = rom[1];
  decoded->immediate2 = rom[2];
  decoded->bytes_used = 1;
  printf("i=%d\top1=%d\top2=%d\n", instr, decoded->op1, decoded->op2);
  switch(instr) {
  case 0:
    if(*rom == 0b00110110) {
      decoded->operation = Instruction::OP_LDHLN;
      decoded->bytes_used = 2;
    } else if(op2 == REG_6 && is_reg(op1)){
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
    } else {
      puts("invalid 00 instruction");
      exit(-1);
    }
    break;
  case 1:
    if(op1 == REG_6 && is_reg(op2)) {
      decoded->operation = Instruction::OP_LDHLR;
    } else if(op2 == REG_6 && is_reg(op1)) {
      decoded->operation = Instruction::OP_LDRHL;
    } else {
      decoded->operation = Instruction::OP_LDRR;
    }
    break;
  case 3:
    if(*rom == 0b11110010) {
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
    }
    break;
  default:
    puts("unable to decode instruction");
    exit(-1);
  }
}
