#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Memory::Memory() {
  size = 0x10000;
  memory = new uint8_t[size];
  memset(memory, 0, size);
}

Memory::~Memory() {
  delete[] memory;
}

uint8_t Memory::read8(int address) const {
  if(address >=0 && address < size) {
    return memory[address];
  }
  puts("attempted read to memory out of bounds");
  exit(-1);
}
uint16_t Memory::read16(int address) {
  puts("not implemented");
  exit(-1);
}

void Memory::write8(int address, uint8_t value) {
  printf("write8(%x, %x)\n", address, value);
  if(address >=0 && address < size) {
    memory[address] = value;
  } else {
    puts("attempted to write to out of bounds memory");
    exit(-1);
  }
}
void Memory::write16(int address, uint16_t value) {
  puts("not implemented");
  exit(-1);
}
