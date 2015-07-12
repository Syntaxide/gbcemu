#include "cpu.h"

#include <stdint.h>
#include "inst_decode.h"
#include "rom.h"

CPU::CPU() {
  reset();
}
CPU::CPU(Rom &rom) {
  this->mRom = &rom;
  reset();
}

void CPU::reset() {
  pc = 0x100;
  a = b = c = d = e = f = h = l = 0;
  sp = 0xfffe;
  flag_z = flag_h = flag_cy = 0;
}

// takes two 8 bit values from memory and composes them together
// currently assumes that the cpu is little endian
uint16_t to16(uint8_t first, uint8_t second) {
  return first + (second << 8);
}

void from16(uint16_t val, uint8_t *msb, uint8_t *lsb) {
  *msb = (val>>8);
  *lsb = (val&0xff);
}

uint8_t isCarrySum(uint8_t fromBit, int first, int second) {
  int lowerMask = (2<<fromBit) -1;
  bool c = ((first&lowerMask) + (second&lowerMask)) & (1 << (fromBit+1));
  return c;
}

bool jumpInstruction(const Instruction &instr) {
  return false;
}

uint8_t *CPU::reg(uint8_t code) {
  switch(code) {
  case 0b111:
    puts("reg a");
    return &a;
  case 0b000:
    return &b;
  case 0b001:
    return &c;
  case 0b010:
    return &d;
  case 0b011:
    return &e;
  case 0b100:
    return &h;
  case 0b101:
    return &l;
  }
  puts("invalid register code");
  exit(-1);
}

uint16_t CPU::BC() const { return (b<<8) + c; }
uint16_t CPU::DE() const { return (d<<8) + e; }
uint16_t CPU::HL() const { return (h<<8) + l; }
uint16_t CPU::AF() const { return (a<<8) + f; }
uint16_t CPU::SP() const { return sp; }

void CPU::setBC(uint16_t value) {
  from16(value, &b, &c);
}
void CPU::setDE(uint16_t value) {
  from16(value, &d, &e);
}
void CPU::setHL(uint16_t value) {
  from16(value, &h, &l);
}
void CPU::setAF(uint16_t value) {
  from16(value, &a, &f);
}
void CPU::setSP(uint16_t value) {
  sp = value;
}
void CPU::setDDPair(uint8_t code, uint16_t value) {
  switch(code) {
    case 0: setBC(value); break;
    case 1: setDE(value); break;
    case 2: setHL(value); break;
    case 3: setSP(value); break;
    default:
            printf("bad reg pair: %d\n", code);
            exit(-1);
  }
}
uint16_t CPU::readDDPair(uint8_t code) const {
  switch(code) {
    case 0: return BC(); break;
    case 1: return DE(); break;
    case 2: return HL(); break;
    case 3: return SP(); break;
    default:puts("READ: INVALID DD CODE");
            exit(-1);
  }
}

void CPU::setQQPair(uint8_t code, uint16_t value) {
  switch(code) {
    case 0: setBC(value); break;
    case 1: setDE(value); break;
    case 2: setHL(value); break;
    case 3: setAF(value); break;
    default:puts("SET: INVALID QQ CODE!");
            exit(-1);
  }
}
uint16_t CPU::readQQPair(uint8_t code) const {
  switch(code) {
    case 0: return BC(); break;
    case 1: return DE(); break;
    case 2: return HL(); break;
    case 3: return AF(); break;
    default:puts("READ: INVALID QQ CODE!");
            exit(-1);
  }
}


void CPU::execute(const Instruction& instr) {
  switch(instr.operation) {
    case Instruction::OP_LDRR:
      *reg(instr.op1) = *reg(instr.op2);
      break;
    case Instruction::OP_LDRN:
      *reg(instr.op1) = instr.immediate;
      break;
    case Instruction::OP_LDRHL:
      *reg(instr.op1) = mem.read8(HL());
      break;
    case Instruction::OP_LDHLR:
      mem.write8(HL(), *reg(instr.op2));
      break;
    case Instruction::OP_LDHLN:
      mem.write8(HL(), instr.immediate);
      break;
    case Instruction::OP_LDABC:
      a = mem.read8(BC());
      break;
    case Instruction::OP_LDADE:
      a = mem.read8(DE());
      break;
    case Instruction::OP_LDAC:
      a = mem.read8(0xff00 + c);
      break;
    case Instruction::OP_LDCA:
      mem.write8(0xff00 + c, a);
      break;
    case Instruction::OP_LDAN:
      a = mem.read8(0xff00 + instr.immediate);
      break;
    case Instruction::OP_LDANN:
      a = mem.read8(to16(instr.immediate, instr.immediate2));
      break;
    case Instruction::OP_LDNNA:
      mem.write8(to16(instr.immediate, instr.immediate2), a);
      break;
    case Instruction::OP_LDAHLI:
      a = mem.read8(HL());
      setHL(a+1);
      break;
    case Instruction::OP_LDAHLD:
      a = mem.read8(HL());
      setHL(a-1);
      break;
    case Instruction::OP_LDBCA:
      mem.write8(BC(), a);
      break;
    case Instruction::OP_LDDEA:
      mem.write8(DE(), a);
      break;
    case Instruction::OP_LDHLIA:
      mem.write8(HL(), a);
      setHL(HL() + 1);
      break;
    case Instruction::OP_LDHLDA:
      mem.write8(HL(), a);
      setHL(HL() - 1);
      break;
    case Instruction::OP_LDDDNN:
      setDDPair(instr.op1>>1, to16(instr.immediate, instr.immediate2));
      break;
    case Instruction::OP_LDSPHL:
      setHL(SP());
      break;
    case Instruction::OP_PUSHQQ:
      {
      uint16_t pair = readQQPair(instr.op1>>1);
      printf("writing value: %x to memory\n", pair);
      mem.write8(sp-1, pair>>8);
      mem.write8(sp-2, pair & 0xff);
      sp -= 2;
      }
      break;
    case Instruction::OP_POPQQ:
      {
        puts("popqq running");
      uint16_t val = to16(mem.read8(sp), mem.read8(sp+1));
      setQQPair(instr.op1>>1, val);
      sp = sp+2;
      }
      break;
    case Instruction::OP_LDHLSP:
      setHL(sp + instr.immediate);
      break;
    case Instruction::OP_LDNNSP:
      {
      uint16_t addr = to16(instr.immediate, instr.immediate2);
      mem.write8(addr, sp & 0xff);
      mem.write8(addr+1, sp >> 8);
      }
      break;
    case Instruction::OP_ADDAR:
      a = alu_add8(a, *reg(instr.op2), 0);
      break;
    case Instruction::OP_ADDAN:
      a = alu_add8(a, instr.immediate, 0);
      break;
    case Instruction::OP_ADDAHL:
      a = alu_add8(a, mem.read8(HL()), 0);
      break;
    case Instruction::OP_ADCAR:
      a = alu_add8(a, *reg(instr.op2), flag_cy);
      break;
    case Instruction::OP_ADCAN:
      a = alu_add8(a, instr.immediate, flag_cy);
      break;
    case Instruction::OP_ADCAHL:
      a = alu_add8(a, mem.read8(HL()), flag_cy); 
      break;
    case Instruction::OP_SUBR:
      a = alu_sub8(a, *reg(instr.op2), 0);
      break;
    case Instruction::OP_SUBN:
      a = alu_sub8(a, instr.immediate, 0);
      break;
    case Instruction::OP_SUBHL:
      a = alu_sub8(a, mem.read8(HL()), 0);
      break;
    case Instruction::OP_SBCAR:
      a = alu_sub8(a, *reg(instr.op2), flag_cy);
      break;
    case Instruction::OP_SBCAN:
      a = alu_sub8(a, instr.immediate, flag_cy);
      break;
    case Instruction::OP_SBCAHL:
      a = alu_sub8(a, mem.read8(HL()), flag_cy);
      break;
    case Instruction::OP_ANDR:
      a = alu_and8(a, *reg(instr.op2));
      break;
    case Instruction::OP_ANDN:
      a = alu_and8(a, instr.immediate);
      break;
    case Instruction::OP_ANDHL:
      a = alu_and8(a, mem.read8(HL()));
      break;
    case Instruction::OP_ORR:
      a = alu_or8(a, *reg(instr.op2));
      break;
    case Instruction::OP_ORN:
      a = alu_or8(a, instr.immediate);
      break;
    case Instruction::OP_ORHL:
      a = alu_or8(a, mem.read8(HL()));
      break;
    case Instruction::OP_XORR:
      a = alu_xor8(a, *reg(instr.op2));
      break;
    case Instruction::OP_XORN:
      a = alu_xor8(a, instr.immediate);
      break;
    case Instruction::OP_XORHL:
      a = alu_xor8(a, mem.read8(HL()));
      break;
    case Instruction::OP_CPR:
      alu_cp8(a, *reg(instr.op2));
      break;
    case Instruction::OP_CPN:
      alu_cp8(a, instr.immediate);
      break;
    case Instruction::OP_CPHL:
      alu_xor8(a, mem.read8(HL()));
      break;
    case Instruction::OP_INCR:
      *reg(instr.op1) = alu_inc8(*reg(instr.op1));
      break;
    case Instruction::OP_INCHL:
      {
      uint8_t next = alu_inc8(mem.read8(HL()));
      mem.write8(HL(), next);
      }
      break;
    case Instruction::OP_DECR:
      *reg(instr.op1) = alu_dec8(*reg(instr.op1));
      break;
    case Instruction::OP_DECHL:
      {
      uint8_t next = alu_dec8(mem.read8(HL()));
      mem.write8(HL(), next);
      }
      break;
    case Instruction::OP_ADDHLSS:
      setHL(alu_add16(HL(), readDDPair(instr.op1>>1), 0));
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
    case Instruction::OP_SRARHL:
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
      pc = to16(instr.immediate, instr.immediate2);
      return;
      break;
    case Instruction::OP_JPCC:
      break;
    case Instruction::OP_JRE:
      break;
    case Instruction::OP_JRCC:
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
    case Instruction::OP_RST:
      break;
    case Instruction::OP_RETCC:
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

  pc += instr.bytes_used;
}

uint8_t CPU::alu_add8(uint8_t first, uint8_t second, uint8_t c) {
  uint8_t sum = first + second + c;
  flag_h = ((first&0xf0) + (second&0xf0)) != (sum&0xf0);  //carry from lower nibble
  flag_cy = ((first + second + c) & 0xf00) != 0;
  flag_z = (sum == 0);
  flag_n = 0;
  return sum;
}

uint16_t CPU::alu_add16(uint16_t first, uint16_t second, uint8_t c) {
  uint16_t sum = first + second;
  flag_h = isCarrySum(11, first, second);
  flag_cy = isCarrySum(15, first, second);
  flag_n = 0;
  return sum;
}
uint8_t CPU::alu_sub8(uint8_t first, uint8_t second, uint8_t c) {
  uint8_t diff = first - c - second;
  flag_h = ((first&0x0f) -c - (second&0x0f)) != (diff&0x0f);  //carry to lower nibble
  flag_cy = ((first - second - c) & 0xf00) != 0;
  flag_z = (diff == 0);
  flag_n = 1;
  return diff;
}
uint8_t CPU::alu_and8(uint8_t first, uint8_t second) {
  uint8_t res = first & second;
  flag_h = 1;
  flag_cy = 0;
  flag_z = (res == 0);
  flag_n = 0;
  return res;
}

uint8_t CPU::alu_or8(uint8_t first, uint8_t second) {
  uint8_t res = first | second;
  flag_h = 0;
  flag_cy = 0;
  flag_z = (res == 0);
  flag_n = 0;
  return res;
}
uint8_t CPU::alu_xor8(uint8_t first, uint8_t second) {
  uint8_t res = first ^ second;
  flag_h = 0;
  flag_cy = 0;
  flag_z = (res == 0);
  flag_n = 0;
  return res;
}
void CPU::alu_cp8(uint8_t first, uint8_t second) {
  alu_sub8(first, second, 0);
}

uint8_t CPU::alu_inc8(uint8_t val) {
  val = val + 1;
  flag_z = (val == 0);
  flag_n = 0;
  flag_h = (val&0xf0) != ((val-1)&0xf0);
  return val;
}

uint8_t CPU::alu_dec8(uint8_t val) {
  val = val - 1;
  flag_z = (val == 0);
  flag_n = 1;
  flag_h = (val&0xf0) != ((val+1)&0xf0);
  return val;
}

void CPU::step() {
  printf("PC: %x\n", pc);
  RomView nearPC(*mRom, pc);
  Instruction instr;
  decode_instruction(nearPC, &instr);
  showOperation(instr.operation);
  printf("%x, %x\n", instr.immediate, instr.immediate2);
  if(instr.operation == Instruction::OP_INVALID) {
    puts("exiting due to bad instruction");
    exit(-1);
  }

  execute(instr);

}

