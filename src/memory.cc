#include "memory.h"
#include <string.h>

Memory::Memory() {
  size = 0x10000;
  memory = new uint8_t[size];
  memset(memory, 0, size);
}

Memory::~Memory() {
  delete[] memory;
}

uint8_t Memory::read8(int address) {
  if(address >=0 && address < size) {
    return memory[address];
  }
  return 0;
}
uint16_t Memory::read16(int address) {
  return 0;
}

void Memory::write8(int address, uint8_t value) {
  if(address >=0 && address < size) {
    memory[address] = value;
  }
}
void Memory::write16(int address, uint16_t value) {
}
