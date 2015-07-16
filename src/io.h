#pragma once
#include "memory.h"
#include "SDL2/SDL.h"
class IO {
  public:
    IO(Memory *memory);
    ~IO();
    void drawAll();
    bool update();
  private:
    void drawBank0();
    void drawBank1();
    Memory *memory;
    SDL_Window *window;
    SDL_Renderer *renderer;
};
