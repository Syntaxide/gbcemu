#include "cpu.h"

#include <stdint.h>
#include "inst_decode.h"
#include "rom.h"

CPU::CPU(Rom &rom) {
  this->mRom = &rom;
  reset();
}

void CPU::reset() {
  pc = 0;
}

bool jumpInstruction(const Instruction &instr) {
  return false;
}

void CPU::step() {
  printf("PC: %x\n", pc);
  RomView nearPC(*mRom, pc);
  Instruction instr;
  decode_instruction(nearPC, &instr);
  showInstruction(instr.operation);
  if(instr.operation == Instruction::OP_INVALID) {
    puts("exiting due to bad instruction");
    exit(-1);
  }

  switch(instr.operation) {
    case Instruction::OP_JPNN:
      pc = instr.immediate + (instr.immediate2 << 8);
      return;
      break;
    default:
     printf("instruction not yet supported: %s\n", OperationStrings[instr.operation]);
  } 


  pc += instr.bytes_used;
}

