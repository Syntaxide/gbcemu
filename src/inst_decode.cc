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

#define GEN_ENUM(OBJ) OBJ,
#define GEN_STRING(OBJ) #OBJ,
#define MAP_OPERATIONS(FUN) \
  /* 8 bit transfer / IO (p85)*/               \
  FUN(OP_LDRR)                                 \
    FUN(OP_LDRN)                               \
    FUN(OP_LDRHL)                              \
    FUN(OP_LDHLR)                              \
    FUN(OP_LDHLN)                              \
    FUN(OP_LDABC)                              \
    FUN(OP_LDADE)                              \
    FUN(OP_LDAC)                               \
    FUN(OP_LDCA)                               \
    FUN(OP_LDAN)                               \
    FUN(OP_LDNA)                               \
    FUN(OP_LDANN)                              \
    FUN(OP_LDNNA)                              \
    FUN(OP_LDAHLI)                             \
    FUN(OP_LDAHLD)                             \
    FUN(OP_LDBCA)                              \
    FUN(OP_LDDEA)                              \
    FUN(OP_LDHLIA)                             \
    FUN(OP_LDHLDA)                           \
    /* 16 bit transfer (p90)*/               \
    FUN(OP_LDDDNN)                        \
    FUN(OP_LDSPHL)                        \
    FUN(OP_PUSHQQ)                        \
    FUN(OP_POPQQ)                         \
    FUN(OP_LDHLSP)                        \
    FUN(OP_LDNNSP)                         \
    /* 8bit arith logical (p92)*/          \
    FUN(OP_ADDAR)                          \
    FUN(OP_ADDAN)                          \
    FUN(OP_ADDAHL)                         \
    FUN(OP_ADCAR)                          \
    FUN(OP_ADCAN)                          \
    FUN(OP_ADCAHL)                         \
    FUN(OP_SUBR)                           \
    FUN(OP_SUBN)                           \
    FUN(OP_SUBHL)                          \
    FUN(OP_SBCAR)                          \
    FUN(OP_SBCAN)                          \
    FUN(OP_SBCAHL)                         \
    FUN(OP_ANDR)                           \
    FUN(OP_ANDN)                           \
    FUN(OP_ANDHL)                          \
    FUN(OP_ORR)                            \
    FUN(OP_ORN)                            \
    FUN(OP_ORHL)                           \
    FUN(OP_XORR)                           \
    FUN(OP_XORN)                           \
    FUN(OP_XORHL)                          \
    FUN(OP_CPR)                            \
    FUN(OP_CPN)                            \
    FUN(OP_CPHL)                           \
    FUN(OP_INCR)                           \
    FUN(OP_INCHL)                          \
    FUN(OP_DECR)                           \
    FUN(OP_DECHL)

static const char *OperationStrings[] = {
    MAP_OPERATIONS(GEN_STRING)
};

struct Instruction {
    enum Operation {
        MAP_OPERATIONS(GEN_ENUM)
    } operation;
    uint8_t op1, op2;\
    uint8_t immediate;\
    uint8_t immediate2;\
    uint8_t bytes_used;\
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
  else if((instr == first) && (op2 == two) && is_reg(op1)){ \
  decoded->operation = label;                       \
  decoded->bytes_used = len;                        \
  }
#define INSTR_REGISTER_REG2(first, one, label, len)                      \
  else if((instr == first) && (op1 == one) && is_reg(op2)){            \
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
    }  else if(instr == 1 && op1 == REG_6 && is_reg(op2)) {
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
      decoded->bytes_used = 2;
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
