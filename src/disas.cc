#include "disas.h"
#include "cpu.h"
#include <stdio.h>

const char* showReg(uint8_t code) {
  const char *regs[] = {"b", "c", "d", "e", "h", "l", "unknown", "a"};
  if(code < 8) {
    return regs[code];
  } else {
    printf("Bad reg code: %d\n", code);
    exit(-1);
  }
}

const char* showCC(uint8_t code) {
  const char *conds[] = {"nz", "z", "nc", "c"};
  if(code < 4) {
    return conds[code];
  } else {
    printf("bad cc code: %d\n", code);
    exit(-1);
  }
}
// pretty print instruction struct, 
// acts like a disassembler
void show(const Instruction &instr) {
  printf("%s ", OperationStrings[instr.operation]);
  switch(instr.operation) {
    case Instruction::OP_LDRR:
      printf("%s, %s", showReg(instr.op1), showReg(instr.op2));
      break;
    case Instruction::OP_LDRN:
      printf("%s, %d", showReg(instr.op1), instr.op2);
      break;
    case Instruction::OP_LDRHL:
      break;
    case Instruction::OP_LDHLR:
      break;
    case Instruction::OP_LDHLN:
      break;
    case Instruction::OP_LDABC:
      break;
    case Instruction::OP_LDADE:
      break;
    case Instruction::OP_LDAC:
      break;
    case Instruction::OP_LDCA:
      break;
    case Instruction::OP_LDAN:
      break;
    case Instruction::OP_LDNA:
      printf("(%x), a", 0xff00+instr.immediate);
      break;
    case Instruction::OP_LDANN:
      break;
    case Instruction::OP_LDNNA:
      break;
    case Instruction::OP_LDAHLI:
      break;
    case Instruction::OP_LDAHLD:
      break;
    case Instruction::OP_LDBCA:
      break;
    case Instruction::OP_LDDEA:
      break;
    case Instruction::OP_LDHLIA:
      break;
    case Instruction::OP_LDHLDA:
      break;
    case Instruction::OP_LDDDNN:
      break;
    case Instruction::OP_LDSPHL:
      break;
    case Instruction::OP_PUSHQQ:
      break;
    case Instruction::OP_POPQQ:
      break;
    case Instruction::OP_LDHLSP:
      break;
    case Instruction::OP_LDNNSP:
      break;
    case Instruction::OP_ADDAR:
      break;
    case Instruction::OP_ADDAN:
      break;
    case Instruction::OP_ADDAHL:
      break;
    case Instruction::OP_ADCAR:
      break;
    case Instruction::OP_ADCAN:
      break;
    case Instruction::OP_ADCAHL:
      break;
    case Instruction::OP_SUBR:
      break;
    case Instruction::OP_SUBN:
      break;
    case Instruction::OP_SUBHL:
      break;
    case Instruction::OP_SBCAR:
      break;
    case Instruction::OP_SBCAN:
      break;
    case Instruction::OP_SBCAHL:
      break;
    case Instruction::OP_ANDR:
      break;
    case Instruction::OP_ANDN:
      break;
    case Instruction::OP_ANDHL:
      break;
    case Instruction::OP_ORR:
      break;
    case Instruction::OP_ORN:
      break;
    case Instruction::OP_ORHL:
      break;
    case Instruction::OP_XORR:
      break;
    case Instruction::OP_XORN:
      break;
    case Instruction::OP_XORHL:
      break;
    case Instruction::OP_CPR:
      break;
    case Instruction::OP_CPN:
      printf("%x", instr.immediate);
      break;
    case Instruction::OP_CPHL:
      break;
    case Instruction::OP_INCR:
      break;
    case Instruction::OP_INCHL:
      break;
    case Instruction::OP_DECR:
      break;
    case Instruction::OP_DECHL:
      break;
    case Instruction::OP_ADDHLSS:
      break;
    case Instruction::OP_ADDSPE:
      break;
    case Instruction::OP_INCSS:
      break;
    case Instruction::OP_DECSS:
      break;
    case Instruction::OP_RLCA:
      break;
    case Instruction::OP_RLA:
      break;
    case Instruction::OP_RRCA:
      break;
    case Instruction::OP_RRA:
      break;
    case Instruction::OP_RLCR:
      break;
    case Instruction::OP_RLCHL:
      break;
    case Instruction::OP_RLR:
      break;
    case Instruction::OP_RLHL:
      break;
    case Instruction::OP_RRCR:
      break;
    case Instruction::OP_RRCHL:
      break;
    case Instruction::OP_RRR:
      break;
    case Instruction::OP_RRHL:
      break;
    case Instruction::OP_SLAR:
      break;
    case Instruction::OP_SLAHL:
      break;
    case Instruction::OP_SRAR:
      break;
    case Instruction::OP_SRAHL:
      break;
    case Instruction::OP_SRLR:
      break;
    case Instruction::OP_SRLHL:
      break;
    case Instruction::OP_SWAPR:
      break;
    case Instruction::OP_SWAPHL:
      break;
    case Instruction::OP_BITR:
      break;
    case Instruction::OP_BITHL:
      break;
    case Instruction::OP_SETR:
      break;
    case Instruction::OP_SETHL:
      break;
    case Instruction::OP_RESR:
      break;
    case Instruction::OP_RESHL:
      break;
    case Instruction::OP_JPNN:
      printf("%x", to16(instr.immediate, instr.immediate2));
      break;
    case Instruction::OP_JPCC:
      printf("%s, %2x%2x", showCC(instr.op1), instr.immediate2, instr.immediate);
      break;
    case Instruction::OP_JRE:
      break;
    case Instruction::OP_JRCC:
      printf("%s, %x", showCC(instr.op1&0b11), instr.immediate);
      break;
    case Instruction::OP_JPHL:
      break;
    case Instruction::OP_CALLNN:
      break;
    case Instruction::OP_CALLCC:
      break;
    case Instruction::OP_RETI:
      break;
    case Instruction::OP_RET:
      break;
    case Instruction::OP_RETCC:
      break;
    case Instruction::OP_RST:
      break;
    case Instruction::OP_DAA:
      break;
    case Instruction::OP_CPL:
      break;
    case Instruction::OP_NOP:
      break;
    case Instruction::OP_HALT:
      break;
    case Instruction::OP_STOP:
      break;
    default:
     printf("instruction not yet supported: %s\n", OperationStrings[instr.operation]);
  } 
  puts("");
}
