#pragma once
#include "memory.h"
#include "SDL2/SDL.h"
class TimeManager {
  public:
    TimeManager() : ticks(0) {
      last_draw = SDL_GetTicks();
    }
    void addCycles(bool doublespeed, uint8_t cycles);
    bool shouldDraw();
    void reset();
    uint32_t last_idle;
  private:
    // when in double speed mode, each cycle counts for one.
    // when in normal speed, 1 cycle = 2 ticks;
    uint32_t ticks;
    uint32_t last_draw;
};

class IO {
  public:
    IO(Memory *memory);
    ~IO();
    void drawAll();
    bool update();
    void addTimeManager(TimeManager *time);
  private:
    void drawTile(int tilenum, int x, int y);
    void drawBank0();
    void drawBank1();
    Memory *memory;
    SDL_Window *window;
    SDL_Renderer *renderer;
    char title[1024];
    TimeManager *time;
};
