#pragma once
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rom.h"

uint8_t select_bits(uint8_t in, char start, char end);
enum reg_encoding {
  REG_b = 0,
  REG_c = 1,
  REG_d = 2,
  REG_e = 3,
  REG_h = 4,
  REG_l = 5,
  REG_6 = 6,
  REG_a = 7
};

#define GEN_ENUM(OBJ, CYCLES) OBJ,
#define GEN_STRING(OBJ, CYCLES) #OBJ,
#define GEN_TIMING(OBJ, CYCLES) CYCLES,
#define MAP_OPERATIONS(FUN) \
  FUN(OP_INVALID, 0) \
  /* 8 bit transfer / IO (p85)*/               \
  FUN(OP_LDRR, 1)                                 \
  FUN(OP_LDRN, 2)                               \
  FUN(OP_LDRHL, 2)                              \
  FUN(OP_LDHLR, 2)                              \
  FUN(OP_LDHLN, 3)                              \
  FUN(OP_LDABC, 2)                              \
  FUN(OP_LDADE, 2)                              \
  FUN(OP_LDAC, 2)                               \
  FUN(OP_LDCA, 2)                               \
  FUN(OP_LDAN, 3)                               \
  FUN(OP_LDNA, 3)                               \
  FUN(OP_LDANN, 4)                              \
  FUN(OP_LDNNA, 4)                              \
  FUN(OP_LDAHLI, 2)                             \
  FUN(OP_LDAHLD, 2)                             \
  FUN(OP_LDBCA, 2)                              \
  FUN(OP_LDDEA, 2)                              \
  FUN(OP_LDHLIA, 2)                             \
  FUN(OP_LDHLDA, 2)                           \
  FUN(OP_LDDDNN, 3)                        \
  FUN(OP_LDSPHL, 2)                        \
  FUN(OP_PUSHQQ, 4)                        \
  FUN(OP_POPQQ, 3)                         \
  FUN(OP_LDHLSP, 3)                        \
  FUN(OP_LDNNSP, 5)                         \
  FUN(OP_ADDAR, 1)                          \
  FUN(OP_ADDAN, 2)                          \
  FUN(OP_ADDAHL, 2)                         \
  FUN(OP_ADCAR, 1)                          \
  FUN(OP_ADCAN, 2)                          \
  FUN(OP_ADCAHL, 2)                         \
  FUN(OP_SUBR, 1)                           \
  FUN(OP_SUBN, 2)                           \
  FUN(OP_SUBHL, 2)                          \
  FUN(OP_SBCAR, 1)                          \
  FUN(OP_SBCAN, 2)                          \
  FUN(OP_SBCAHL, 2)                         \
  FUN(OP_ANDR, 1)                           \
  FUN(OP_ANDN, 2)                           \
  FUN(OP_ANDHL, 2)                          \
  FUN(OP_ORR, 1)                            \
  FUN(OP_ORN, 2)                            \
  FUN(OP_ORHL, 2)                           \
  FUN(OP_XORR, 1)                           \
  FUN(OP_XORN, 2)                           \
  FUN(OP_XORHL, 2)                          \
  FUN(OP_CPR, 1)                            \
  FUN(OP_CPN, 2)                            \
  FUN(OP_CPHL, 2)                           \
  FUN(OP_INCR, 1)                           \
  FUN(OP_INCHL, 3)                          \
  FUN(OP_DECR, 1)                           \
  FUN(OP_DECHL, 3)                            \
  FUN(OP_ADDHLSS, 2)                          \
  FUN(OP_ADDSPE, 4)                           \
  FUN(OP_INCSS, 2)                            \
  FUN(OP_DECSS, 2)                            \
  FUN(OP_RLCA, 1)                             \
  FUN(OP_RLA, 1)                              \
  FUN(OP_RRCA, 1)                             \
  FUN(OP_RRA, 1)                              \
  FUN(OP_RLCR, 2)                             \
  FUN(OP_RLCHL, 4)                            \
  FUN(OP_RLR, 2)                              \
  FUN(OP_RLHL, 4)    \
  FUN(OP_RRCR, 2)		\
  FUN(OP_RRCHL, 4)		\
  FUN(OP_RRR, 2)		  \
  FUN(OP_RRHL, 4)    \
  FUN(OP_SLAR, 2)    \
  FUN(OP_SLAHL, 4)   \
  FUN(OP_SRAR, 2)    \
  FUN(OP_SRAHL, 4)  \
  FUN(OP_SRLR, 2)    \
  FUN(OP_SRLHL, 4)   \
  FUN(OP_SWAPR, 2)   \
  FUN(OP_SWAPHL, 4)  \
  FUN(OP_BITR, 2)    \
  FUN(OP_BITHL, 4)   \
  FUN(OP_SETR, 2)    \
  FUN(OP_SETHL, 4)   \
  FUN(OP_RESR, 2)    \
  FUN(OP_RESHL, 4)   \
  FUN(OP_JPNN, 4)    \
  FUN(OP_JPCC, 4)    \
  FUN(OP_JRE, 3)     \
  FUN(OP_JRCC, 3)   \
  FUN(OP_JPHL, 1)    \
  FUN(OP_CALLNN, 6)  \
  FUN(OP_CALLCC, 6)   \
  FUN(OP_RET, 4)     \
  FUN(OP_RETI, 4)    \
  FUN(OP_RETCC, 5)   \
  FUN(OP_RST, 4)     \
  FUN(OP_DAA, 1)     \
  FUN(OP_CPL, 1)     \
  FUN(OP_NOP, 1)     \
  FUN(OP_HALT, 1)    \
  FUN(OP_STOP, 1)

static const char *OperationStrings[] = {
    MAP_OPERATIONS(GEN_STRING)
};

static uint8_t TimingInfo[] = {
  MAP_OPERATIONS(GEN_TIMING)
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

void showOperation(Instruction::Operation op);
uint8_t getCycles(Instruction::Operation op, bool didJump);

void decode_instruction(const RomViewBase &rom, Instruction *decoded);
