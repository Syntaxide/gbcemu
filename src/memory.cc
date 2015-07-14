#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Memory::Memory() {
  size = 0x10000;
  memory = new uint8_t[size];
  memset(memory, 0, size);
  init();
}

Memory::~Memory() {
  delete[] memory;
}

void Memory::init() {
  write8(0xff10, 0x80);
  write8(0xff11, 0xbf);
  write8(0xff12, 0xf3);
  write8(0xff14, 0xbf);
  write8(0xff16, 0x3f);
  write8(0xFF17,0x00);
  write8(0xFF19,0xBF);
  write8(0xFF1A,0x7F);
  write8(0xFF1B,0xFF);
  write8(0xFF1C,0x9F);
  write8(0xFF1E,0xBF);
  write8(0xFF20,0xFF);
  write8(0xFF21,0x00);
  write8(0xFF22,0x00);
  write8(0xFF23,0xBF);
  write8(0xFF24,0x77);
  write8(0xFF25,0xF3);
  write8(0xFF26,0xF1);
  write8(0xFF40,0x91);
  write8(0xFF42,0x00);
  write8(0xFF43,0x00);
  write8(0xFF45,0x00);
  write8(0xFF47,0xFC);
  write8(0xFF48,0xFF);
  write8(0xFF49,0xFF);
  write8(0xFF4A,0x00);
  write8(0xFF4B,0x00);
  write8(0xFFFF,0x00);
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
  //printf("write8(%x, %x)\n", address, value);
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
