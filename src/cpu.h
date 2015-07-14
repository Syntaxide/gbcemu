#pragma once
#include <stdint.h>
#include "inst_decode.h"
#include "rom.h"
#include "memory.h"

enum reg_addrs {
  reg_p1  = 0xff00,
  reg_sb  = 0xff01,
  reg_sc  = 0xff02,
  reg_div = 0xff04,
  reg_tima = 0xff05,
  reg_tma = 0xff06,
  reg_tac = 0xff07,
  reg_key1 = 0xff4d,
  reg_rp = 0xff56,

  reg_vbk = 0xff4f,
  reg_svbk = 0xff70,
  
  reg_if = 0xff0f,
  reg_ie = 0xffff,
  //reg_ime
  reg_lcdc = 0xff40,
  reg_stat = 0xff41,
  reg_scy = 0xff42,
  reg_scx = 0xff43,
  reg_ly = 0xff44,
  reg_lyc = 0xff45,
  reg_dma = 0xff46,
  reg_bgp = 0xff47,
  reg_opb0 = 0xff48,
  reg_obp1 = 0xff49,
  reg_wy = 0xff4a,
  reg_wx = 0xff4b,
  reg_hdma1 = 0xff51,
  reg_hdma2 = 0xff52,
  reg_hdma3 = 0xff53,
  reg_hdma4 = 0xff54,
  reg_hdma5 = 0xff55,
  reg_bcps = 0xff68,
  reg_bcpd = 0xff69,
  reg_ocps = 0xff6a,
  reg_ocpd = 0xff6b
};

enum CPU_Speed {
  SPEED_NORMAL = 0, //1.05 Mhz
  SPEED_DOUBLE, //2.10 Mhz
};

class CPU {
public:
  CPU();
  CPU(Rom &rom);
  void reset();
    
  void step();
  void execute(const Instruction &instr);

  uint8_t a, b, c, d, e, f, g, h, l;
  //uint8_t flag_z, flag_h, flag_n, flag_cy;
  bool Z();
  bool H();
  bool N();
  bool CY();
  void setZ(bool v);
  void setH(bool v);
  void setN(bool v);
  void setCY(bool v);
  uint16_t pc, sp;

  // sets carry flags appropriately
  uint8_t alu_add8(uint8_t first, uint8_t second, uint8_t c);
  uint16_t alu_add16(uint16_t first, uint16_t second, uint8_t c);
  uint8_t alu_sub8(uint8_t first, uint8_t second, uint8_t c);
  uint8_t alu_and8(uint8_t first, uint8_t second);
  uint8_t alu_or8(uint8_t first, uint8_t second);
  uint8_t alu_xor8(uint8_t first, uint8_t second);
  void alu_cp8(uint8_t first, uint8_t second);
  uint8_t alu_inc8(uint8_t val);
  uint8_t alu_dec8(uint8_t val);
  uint8_t alu_rr(uint8_t val);
  uint8_t alu_rrc(uint8_t val);
  uint8_t alu_rl(uint8_t val);
  uint8_t alu_rlc(uint8_t val);
  uint8_t alu_sla(uint8_t val);
  uint8_t alu_sra(uint8_t val);
  uint8_t alu_srl(uint8_t val);
  uint8_t alu_swap(uint8_t val);
  void alu_bit(uint8_t val, uint8_t bit);
  uint8_t alu_set(uint8_t val, uint8_t bit);
  uint8_t alu_res(uint8_t val, uint8_t bit);
  bool jpcc(uint8_t cond);
  void pushQQ(uint16_t value);
  uint16_t popQQ();
  void call(uint16_t dest);
  void ret();
  Memory mem;
  // get value of pairs
  uint16_t BC() const;
  uint16_t DE() const;
  uint16_t HL() const;
  uint16_t AF() const;
  uint16_t SP() const;
  void setBC(uint16_t val);
  void setDE(uint16_t val);
  void setHL(uint16_t val);
  void setAF(uint16_t val);
  void setSP(uint16_t val);
  void setDDPair(uint8_t code, uint16_t value);
  uint16_t readDDPair(uint8_t code) const;
  void setQQPair(uint8_t code, uint16_t value);
  uint16_t readQQPair(uint8_t code) const;
private:
  /* 
     core of execution. there are some things
     that must be done for each instruction, so these are wrapped in execute()
    */
  void execute2(const Instruction &instr);
  // takes a reg code, returns a reference to the member.
  // useful so that reg(0b111) = a;
  uint8_t *reg(uint8_t code);

  Rom *mRom;
  CPU_Speed speed;
  Instruction instruction;
};
