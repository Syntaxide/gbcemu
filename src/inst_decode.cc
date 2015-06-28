/*
  MSB ..... LSB

  registers:
  000 - b
  001 - c
  010 - NOT USED
  011 - e
  100 - h
  101 - d ? SAME?!??!
  101 - l
  110 - RESERVED, see instruction formats
  111 - a
  LD - p85
  LD : 00 R N
  LD : 01 R R'
*/

#include <stdint.h>

enum reg_encodings {
  REG_b = 0,
  REG_c = 1,
  REG_UNUSED = 2,
  REG_e = 3,
  REG_h = 4,
  REG_d = 5,
  REG_RESV = 6,
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

void decode_instruction(char *rom, bool listOnly = true) {
  uint8_t instr = (*rom) & MASK_76;
  uint8_t op1 = (*rom) & MASK_543;
  uint8_t op2 = (*rom) & MASK_210;
}
