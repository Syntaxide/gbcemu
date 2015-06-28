#include <stdint.h>

enum special_mem_names {
  reg_p1 = 0, //ff00 - io ports
  reg_sb, //ff01
  reg_sc, //ff02
  reg_div, //ff04
  reg_tima, //FF05
  reg_tma, //FF06
  reg_tac, //ff07
  reg_if, //ff0f
  reg_lcdc, //ff40 - lcd control
  reg_stat, //ff41 - lcd status
  reg_scy, //ff42 - scroll y
  reg_scx, //ff43 - scroll x
  reg_ly, //ff44 - lcdc y coord. 0-153=draw, 144-153=blanking
  reg_lyc, //ff45 - ly compare
  reg_wy, //ff4a -  window y
  reg_wx, //ff4b - window x
  reg_key1, //ff4d - 
  reg_rp, //ff56 - infrared
  reg_bcps, //ff68 - bg write
  reg_bcpd, //ff69 - bg write data
  reg_ocps, //ff6a - obj write
  reg_ocpd, //ff6b - obj write data
  reg_ie, //ffff
  reg_wy,
  reg_w,
  reg_stack,
  SPECIAL_MEM_COUNT
};

enum registers {
  reg_a = 0,
  reg_b,
  reg_c,
  reg_d,
  reg_e,
  reg_f,
  reg_h,
  reg_l,
  reg_pc,
  reg_sp,
  reg_flags,
};

enum CPU_Speed {
  SPEED_NORMAL = 0, //1.05 Mhz
  SPEED_DOUBLE, //2.10 Mhz
};

class CPU {
public:
private:

  uint8_t registers[NUM_REGISTERS];
  CPU_Speed speed;
};
