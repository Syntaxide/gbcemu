#include "inst_decode.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rom.h"

// indices   76543210
// select(0xb10101010)
uint8_t select_bits(uint8_t in, char start, char end) {
  uint8_t ones = ((1 << (start-end+1)) - 1) << end;
  return (in & ones) >> end;
}

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

void showOperation(Instruction::Operation op) {
  puts(OperationStrings[op]);
}

void decode_instruction(const RomViewBase &rom, Instruction *decoded) {
  uint8_t byte = rom[0];
  uint8_t instr = select_bits(byte, 7, 6);
  uint8_t op1 = decoded->op1 = select_bits(byte, 5, 3);
  uint8_t op2 = decoded->op2 = select_bits(byte, 2, 0);
  decoded->operation = Instruction::OP_INVALID;
  decoded->immediate = rom[1];
  decoded->immediate2 = rom[2];
  decoded->bytes_used = 1;
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
    INSTR_REGISTER_REG2(1, 0b110, Instruction::OP_LDHLR, 1)
    INSTR_REGISTER_REG(1, 0b110, Instruction::OP_LDRHL, 1)
    else if(instr == 0b01 && is_reg(op1) && is_reg(op2)){
      decoded->operation = Instruction::OP_LDRR;
    } 
    INSTR_REGISTER_REG2(2, 2, Instruction::OP_SUBR, 1)
    INSTR_REGISTER_REG2(2, 3, Instruction::OP_SBCAR, 1)
    INSTR_REGISTER_DEF(0b10011110, Instruction::OP_SBCAHL, 1)
    INSTR_REGISTER_DEF(0b10100110, Instruction::OP_ANDHL, 1)
    INSTR_REGISTER_REG2(2, 0b110, Instruction::OP_ORR, 1)
    INSTR_REGISTER_DEF(0b10110110, Instruction::OP_ORHL, 1)
    INSTR_REGISTER_REG2(2, 5, Instruction::OP_XORR, 1)
    INSTR_REGISTER_DEF(0b10101110, Instruction::OP_XORHL, 1)
    INSTR_REGISTER_REG2(2, 7, Instruction::OP_CPR, 1)
    INSTR_REGISTER_DEF(0b10111110, Instruction::OP_CPHL, 1)
    INSTR_REGISTER_REG2(2, 0, Instruction::OP_ADDAR, 1)
    INSTR_REGISTER_DEF(0b10000110,Instruction::OP_ADDAHL, 1)
    INSTR_REGISTER_REG2(2, 1, Instruction::OP_ADCAR, 1)
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
      uint8_t instr = select_bits(byte, 7, 6);
      uint8_t op1 = select_bits(byte, 5, 3);
      uint8_t op2 = select_bits(byte, 2, 0);
      if(byte == 0b00011110) {
        decoded->operation = Instruction::OP_RRHL;
        decoded->bytes_used = 2;
      }
      INSTR_REGISTER_DEF(0b00100110, Instruction::OP_SLAHL, 2)
    }


}
