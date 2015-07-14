#pragma once
#include <stdint.h>
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
