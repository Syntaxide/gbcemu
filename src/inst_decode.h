#pragma once
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rom.h"
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

#define GEN_ENUM(OBJ) OBJ,
#define GEN_STRING(OBJ) #OBJ,
#define MAP_OPERATIONS(FUN) \
  FUN(OP_INVALID) \
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
  FUN(OP_LDDDNN)                        \
  FUN(OP_LDSPHL)                        \
  FUN(OP_PUSHQQ)                        \
  FUN(OP_POPQQ)                         \
  FUN(OP_LDHLSP)                        \
  FUN(OP_LDNNSP)                         \
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

void showOperation(Instruction::Operation op);

void decode_instruction(const RomViewBase &rom, Instruction *decoded);
