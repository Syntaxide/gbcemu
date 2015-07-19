#pragma once
#include <stdint.h>

enum mem_regs {
  P1 = 0xff00,
  SB = 0xFF01,
    SC = 0xFF02,
    DIV = 0xFF04,
    TIMA = 0xFF05,
    TMA = 0xFF06,
    TAC = 0xFF07,
    KEY1 = 0xFF4D,
    RP = 0xFF56,
    VBK = 0xFF4F,
    SVBK = 0xFF70,
    IF = 0xFF0F,
    IE = 0xFFFF,
    LCDC = 0xFF40,
    STAT = 0xFF41, 
    SCY = 0xFF42, 
    SCX = 0xFF43, 
    LY = 0xFF44, 
    LYC = 0xFF45, 
    DMA = 0xFF46, 
    BGP = 0xFF47, 
    OBP0 = 0xFF48, 
    OBP1 = 0xFF49, 
    WY = 0xFF4A, 
    WX = 0xFF4B, 
    HDMA1 = 0xFF51,
    HDMA2 = 0xFF52,
    HDMA3 = 0xFF53,
    HDMA4 = 0xFF54,
    HDMA5 = 0xFF55,
    BCPS = 0xFF68,
    BCPD = 0xFF69,
    OCPS = 0xFF6A,
    OCPD = 0xFF6B,
    OAM_START=0xFE00,
    OAM_END=0xFE9f,
    NR_START = 0xFF10,
    NR_END = 0xFF26,
    WAVEFORM_START = 0xFF30,
    WAVEFORM_END = 0xFF3F
};

class Memory {
  public:
    Memory();
    ~Memory();
    uint8_t read8(int address) const;
    uint16_t read16(int address) ;
    void write8(int address, uint8_t value);
    void write16(int address, uint16_t value);
  private:
    void init();
    uint8_t *memory;
    long size;
};
