#pragma once
#include "memory.h"
class IO {
  public:
    IO(Memory *memory);
    ~IO();
    void drawAll();
    bool update();
  private:
    Memory *memory;
};
