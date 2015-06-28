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
    OP_LDRR,
    OP_LDRN,
    OP_LDRHL,
    OP_LDHLR,
    OP_LDHLN,
    OP_LDABC,
    OP_LDADE,
    OP_LDAC
  } operation;
  uint8_t op1, op2;
  uint8_t immediate;
  uint8_t bytes_used;
};

void decode_instruction(unsigned char *rom, Instruction *decoded) {
  uint8_t instr = select_bits(*rom, 7, 6);
  uint8_t op1 = select_bits(*rom, 5, 3);
  uint8_t op2 = select_bits(*rom, 2, 0);
  printf("i=%d\top1=%d\top2=%d\n", instr, op1, op2);
  memset(decoded, 0, sizeof(Instruction));
  switch(instr) {
  case 0:
    if(op1 == REG_6 && op2 == REG_6) {
      puts("LD (HL) N");
    } else if(op2 == REG_6){
      puts("LD R N");
      rom++;
    } else if(op1 == REG_a && op2 == REG_2) {
      puts("LD A (BC)");
    } else if(op1 == 3 && op2 == REG_2) {
      puts("LD A, (DE)");
    }
    break;
  case 1:
    if(op1 == REG_6) {
      puts("LD (HL) R");
    } else if(op2 == REG_6)
      puts("LD R (HL)");
    else {
      puts("LD R R");
    }
    break;
  default:
    puts("unable to decode instruction");
    exit(-1);
  }
}
