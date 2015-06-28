#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
enum reg_encodings {
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

unsigned char *decode_instruction(unsigned char *rom, bool listOnly = true) {
  uint8_t instr = select_bits(*rom, 7, 6);
  uint8_t op1 = select_bits(*rom, 5, 3);
  uint8_t op2 = select_bits(*rom, 2, 0);
  printf("i=%d\top1=%d\top2=%d\n", instr, op1, op2);
  switch(instr) {
  case 0:
    if(op1 == REG_6 && op2 == REG_6)
      puts("LD (HL) N");
    else if(op2 == REG_6)
      puts("LD R N");
    else if(op1 == REG_a && op2 == REG_2)
      puts("LD A (BC)");
    break;
  case 1:
    if(op1 == REG_6)
      puts("LD (HL) R");
    else if(op2 == REG_6)
      puts("LD R (HL)");
    else
      puts("LD R R");
    break;
  default:
    puts("unable to decode instruction");
    exit(-1);
  }
  return ++rom;
}
