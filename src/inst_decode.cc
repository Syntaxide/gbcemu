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
  FUN(OP_DECHL)                            \
  FUN(OP_ADDHLSS)                          \
  FUN(OP_ADDSPE)                           \
  FUN(OP_INCSS)                            \
  FUN(OP_DECSS)                            \
  FUN(OP_RLCA)                             \
  FUN(OP_RLA)                              \
  FUN(OP_RRCA)                             \
  FUN(OP_RRA)                              \
  FUN(OP_RLCR)                             \
  FUN(OP_RLCHL)                            \
  FUN(OP_RLR)                              \
  FUN(OP_RLHL)    \
  FUN(OP_RRCR)		\
  FUN(OP_RRCHL)		\
  FUN(OP_RRR)		  \
  FUN(OP_RRHL)    \
  FUN(OP_SLAR)    \
  FUN(OP_SLAHL)   \
  FUN(OP_SRAR)    \
  FUN(OP_SRARHL)  \
  FUN(OP_SRLR)    \
  FUN(OP_SRLHL)   \
  FUN(OP_SWAPR)   \
  FUN(OP_SWAPHL)  \
  FUN(OP_BITR)    \
  FUN(OP_BITHL)   \
  FUN(OP_SETR)    \
  FUN(OP_SETHL)   \
  FUN(OP_RESR)    \
  FUN(OP_RESHL)   \
  FUN(OP_JPNN)    \
  FUN(OP_JPCC)    \
  FUN(OP_JRE)     \
  FUN(OP_JRCC)   \
  FUN(OP_JPHL)    \
  FUN(OP_CALLNN)  \
  FUN(OP_CALLCC)   \
  FUN(OP_RETI)    \
  FUN(OP_RET)     \
  FUN(OP_RST)     \
  FUN(OP_RETCC)   \
  FUN(OP_DAA)     \
  FUN(OP_CPL)     \
  FUN(OP_NOP)     \
  FUN(OP_HALT)    \
  FUN(OP_STOP)

	

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
  else if(byte == binary) {                           \
    decoded->operation = label;                       \
    decoded->bytes_used = len;                        \
  }
#define INSTR_REGISTER_REG(first, two, label, len)      \
  else if((select_bits(byte, 7,6) == first) && 		\
	(select_bits(byte, 2,0) == two) &&		\
	 is_reg(select_bits(byte, 5, 3))){		\
    decoded->operation = label;                       	\
    decoded->bytes_used = len;                        	\
  }

#define INSTR_REGISTER_REG2(first, one, label, len)     \
  else if((select_bits(byte, 7,6) == first) &&		\
	 (select_bits(byte, 5, 3) == one) &&		\
	is_reg(select_bits(byte, 2, 0))){		\
    decoded->operation = label; 			\
    decoded->bytes_used = len; 				\
  }

#define INSTR_REGISTER_SSMASK(postmask, label, len) \
  else if((byte & 0b11001111) == postmask){\
    decoded->operation = label;\
    decoded->bytes_used = len;\
  }

#define INSTR(label, len)\
  decoded->operation = label;\
  decoded->bytes_used = len;


void decode_instruction(unsigned char *rom, Instruction *decoded) {
  memset(decoded, 0, sizeof(Instruction));
  uint8_t instr = select_bits(*rom, 7, 6);
  uint8_t op1 = decoded->op1 = select_bits(*rom, 5, 3);
  uint8_t op2 = decoded->op2 = select_bits(*rom, 2, 0);
  decoded->operation = (Instruction::Operation)0xff;
  decoded->immediate = rom[1];
  decoded->immediate2 = rom[2];
  decoded->bytes_used = 1;
  unsigned char byte = rom[0];
  printf("i=%d\top1=%d\top2=%d\n", instr, decoded->op1, decoded->op2);
    if(byte == 0b00110110) {
      decoded->operation = Instruction::OP_LDHLN;
      decoded->bytes_used = 2;
    } 
    INSTR_REGISTER_REG(0, 0b110, Instruction::OP_LDRN, 2)
    INSTR_REGISTER_DEF(0b00001010, Instruction::OP_LDABC, 1)
    INSTR_REGISTER_DEF(0b00011010, Instruction::OP_LDADE, 1)
    INSTR_REGISTER_DEF(0b00101010, Instruction::OP_LDAHLI, 1)
    INSTR_REGISTER_DEF(0b00111010, Instruction::OP_LDAHLD, 1)
    INSTR_REGISTER_DEF(0b00000010, Instruction::OP_LDBCA, 1)
    INSTR_REGISTER_DEF(0b00010010, Instruction::OP_LDDEA, 1)
    INSTR_REGISTER_DEF(0b00100010, Instruction::OP_LDHLIA, 1)
    INSTR_REGISTER_DEF(0b00110010, Instruction::OP_LDHLDA, 1)
    else if((byte & 0b11001111) == 1) {
      decoded->operation = Instruction::OP_LDDDNN;
      decoded->bytes_used = 3;
    }
    INSTR_REGISTER_DEF(0b00001000, Instruction::OP_LDNNSP, 3)
    else if(instr == 1 && op1 == REG_6 && is_reg(op2)) {
      decoded->operation = Instruction::OP_LDHLR;
    } else if(instr == 1 && op2 == REG_6 && is_reg(op1)) {
      decoded->operation = Instruction::OP_LDRHL;
    } else if(instr == 0b01 && is_reg(op1) && is_reg(op2)){
      decoded->operation = Instruction::OP_LDRR;
    } else if(instr == 2 && op1 == 2 && is_reg(op2)) {
      decoded->operation = Instruction::OP_SUBR;
    } else if(instr == 2 && op1 == 3 && is_reg(op2)) {
      decoded->operation = Instruction::OP_SBCAR;
    } else if(byte == 0b10011110) {
      decoded->operation = Instruction::OP_SBCAHL;
    } else if(byte == 0b10100110) {
      decoded->operation = Instruction::OP_ANDHL;
    } else if(instr == 2 && op1 == 6 && is_reg(op2)) {
      decoded->operation = Instruction::OP_ORR;
    } else if(byte == 0b10110110) {
      decoded->operation = Instruction::OP_ORHL;
    } else if(instr == 2 && op1 == 5 && is_reg(op2)) {
      decoded->operation = Instruction::OP_XORR;
    } else if(byte == 0b10101110) {
      decoded->operation = Instruction::OP_XORHL;
    } else if(instr == 2 && op1 == 7 && is_reg(op2)) {
      decoded->operation = Instruction::OP_CPR;
    }
    INSTR_REGISTER_DEF(0b10111110, Instruction::OP_CPHL, 1)
    else if(instr == 2 && op1 == 0 && is_reg(op2)) {
      decoded->operation = Instruction::OP_ADDAR;
    } else if(instr == 2 && byte == 0b10000110) {
      decoded->operation = Instruction::OP_ADDAHL;
    } else if(instr == 2 && op1 == 1 && is_reg(op2)) {
      decoded->operation = Instruction::OP_ADCAR;
    }
    INSTR_REGISTER_DEF(0b10001110, Instruction::OP_ADCAHL, 1)
    INSTR_REGISTER_DEF(0b11110010, Instruction::OP_LDAC, 1)
    INSTR_REGISTER_DEF(0b11100010, Instruction::OP_LDCA, 1)
    INSTR_REGISTER_DEF(0b11110000, Instruction::OP_LDAN, 2)
    INSTR_REGISTER_DEF(0b11100000, Instruction::OP_LDNA, 2)
    INSTR_REGISTER_DEF(0b11111010, Instruction::OP_LDANN, 3)
    INSTR_REGISTER_DEF(0b11101010, Instruction::OP_LDNNA, 3)
    INSTR_REGISTER_DEF(0b11111001, Instruction::OP_LDSPHL, 1)
    INSTR_REGISTER_DEF(0b11000101, Instruction::OP_PUSHQQ, 1)
    INSTR_REGISTER_DEF(0b11000001, Instruction::OP_POPQQ, 1)
    INSTR_REGISTER_DEF(0b11111000, Instruction::OP_LDHLSP, 2)
    INSTR_REGISTER_DEF(0b11010110, Instruction::OP_SUBN, 2)
    INSTR_REGISTER_DEF(0b10010110, Instruction::OP_SUBHL, 1)
    INSTR_REGISTER_DEF(0b11011110, Instruction::OP_SBCAN, 2)
    INSTR_REGISTER_DEF(0b11100110, Instruction::OP_ANDN, 2)
    INSTR_REGISTER_DEF(0b11110110, Instruction::OP_ORN, 2)
    INSTR_REGISTER_DEF(0b11101110, Instruction::OP_XORN, 2)
    INSTR_REGISTER_DEF(0b11111110, Instruction::OP_CPN, 2)
    INSTR_REGISTER_DEF(0b11000110, Instruction::OP_ADDAN, 2)
    INSTR_REGISTER_DEF(0b11001110, Instruction::OP_ADCAN, 2)
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

    INSTR_REGISTER_SSMASK(0b00001001, Instruction::OP_ADDHLSS, 1)
    INSTR_REGISTER_DEF(0b11101000, Instruction::OP_ADDSPE, 2)
    INSTR_REGISTER_SSMASK(0b00000011, Instruction::OP_INCSS, 1)
    INSTR_REGISTER_SSMASK(0b00001011, Instruction::OP_DECSS, 1)

    INSTR_REGISTER_DEF(0b00000111, Instruction::OP_RLCA, 1)
    INSTR_REGISTER_DEF(0b00010111, Instruction::OP_RLA, 1)
    INSTR_REGISTER_DEF(0b00001111, Instruction::OP_RRCA, 1)
    INSTR_REGISTER_DEF(0b00011111, Instruction::OP_RRA, 1)

    INSTR_REGISTER_DEF(0b11000011, Instruction::OP_JPNN, 3)
    else if(instr == 0b11 && (op1 & 0b100) == 0 && op2 == 0b010) {
      INSTR(Instruction::OP_JPCC, 3)
    }
    INSTR_REGISTER_DEF(0b00011000, Instruction::OP_JRE, 2)
    else if(instr == 0 && (op1 & 0b100) && op2 == 0) {
      INSTR(Instruction::OP_JRCC, 2)
    } 
    INSTR_REGISTER_DEF(0b11101001, Instruction::OP_JPHL, 1)
    INSTR_REGISTER_DEF(0b11001101, Instruction::OP_CALLNN, 3)
    else if(instr == 0b11 && !op1 && op2 == 0b100) {
      INSTR(Instruction::OP_CALLCC, 3)
    }
    INSTR_REGISTER_DEF(0b11001001, Instruction::OP_RET, 1)
    else if(instr == 0b11 && !(op1&0b100) && op2 == 0) {
      INSTR(Instruction::OP_RETCC, 1)
    }
    INSTR_REGISTER_DEF(0b11011001, Instruction::OP_RETI, 1)
    INSTR_REGISTER_DEF(0b11111111, Instruction::OP_RST, 1)
    INSTR_REGISTER_DEF(0b00100111, Instruction::OP_DAA, 1)
    INSTR_REGISTER_DEF(0b00101111, Instruction::OP_CPL, 1)
    INSTR_REGISTER_DEF(0, Instruction::OP_NOP, 1)
    INSTR_REGISTER_DEF(0b01110110, Instruction::OP_HALT, 1)
    else if(byte == 0b00010000) {
      if(rom[1] == 0) {
        INSTR(Instruction::OP_STOP, 2)
      }
    }
    else if(byte == 0b11001011) {
      unsigned char byte = rom[1];
      uint8_t instr = select_bits(byte, 7, 6);
      uint8_t op1 = select_bits(byte, 5, 3);
      uint8_t op2 = select_bits(byte, 2, 0);
      if((byte & 0b11111000) == 0 && is_reg(byte & 0b111)) {
        decoded->operation = Instruction::OP_RLCR;
        decoded->bytes_used = 2;
      }
      INSTR_REGISTER_DEF(0b00000110, Instruction::OP_RLCHL, 2)
      INSTR_REGISTER_REG2(0, 0b010, Instruction::OP_RLR, 2)
      INSTR_REGISTER_DEF(0b00010110, Instruction::OP_RLHL, 2)
      INSTR_REGISTER_REG2(0, 0b001, Instruction::OP_RRCR, 2)
      INSTR_REGISTER_DEF(0b00001110, Instruction::OP_RRCHL, 2)
      INSTR_REGISTER_REG2(0, 0b011, Instruction::OP_RRR, 2)
      INSTR_REGISTER_REG2(0, 0b100, Instruction::OP_SLAR, 2)
      INSTR_REGISTER_REG2(0, 0b101, Instruction::OP_SRAR, 2)
      INSTR_REGISTER_DEF(0b00101110, Instruction::OP_SRARHL, 2)
      INSTR_REGISTER_REG2(0, 0b111, Instruction::OP_SRLR, 2)
      INSTR_REGISTER_DEF(0b00111110, Instruction::OP_SRLHL, 2)
      INSTR_REGISTER_REG2(0, 0b110, Instruction::OP_SWAPR, 2)
      INSTR_REGISTER_DEF(0b00110110, Instruction::OP_SWAPHL, 2)
      else if(instr == 0b01 && is_reg(op2)) {
        decoded->operation = Instruction::OP_BITR;
        decoded->bytes_used = 2;
      } else if(instr == 0b01 && op2 == 0b110) {
        INSTR(Instruction::OP_BITHL, 2)
      } else if(instr == 0b11 && is_reg(op2)) {
        INSTR(Instruction::OP_SETR, 2)
      } else if(instr == 0b11 && op2 == 0b110) {
        INSTR(Instruction::OP_SETHL, 2)
      } else if(instr == 0b10 && is_reg(op2)) {
        INSTR(Instruction::OP_RESR, 2)
      } else if(instr == 0b10 && op2 == 0b110) {
        INSTR(Instruction::OP_RESHL, 2)
      }

    } else if(byte == 0b11011011) {
      unsigned char byte = rom[1];
      uint8_t instr = select_bits(*rom, 7, 6);
      uint8_t op1 = select_bits(*rom, 5, 3);
      uint8_t op2 = select_bits(*rom, 2, 0);
      if(byte == 0b00011110) {
        decoded->operation = Instruction::OP_RRHL;
        decoded->bytes_used = 2;
      }
      INSTR_REGISTER_DEF(0b00100110, Instruction::OP_SLAHL, 2)
    }


}
