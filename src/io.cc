#include "io.h"
#include "SDL2/SDL.h"

const int CBANK_0X = 0;
const int CBANK_0Y = 0;
const int CBANK_1X = 128;
const int CBANK_1Y = 0;


char select_bit(uint8_t val, uint8_t bit) {
  return (val & (1<<bit)) >> bit;
}
void TimeManager::addCycles(bool doublespeed, uint8_t cycles) {
  if(doublespeed)
    ticks += cycles;
  else
    ticks += cycles*2;
}
bool TimeManager::shouldDraw() {
  return ticks > 32886;
}

void TimeManager::reset() {
  ticks = 0;
  uint32_t now = SDL_GetTicks();
  uint32_t delta = now - last_draw;
  if(delta > 15) {
    printf("emulator running behind. delta=%d\n", delta);
  } else {
    last_idle = 15 - delta;
    printf("sleeping for %dms\n", last_idle);
    SDL_Delay(last_idle);

  }
  last_draw = now;
}

IO::IO(Memory *memory) {
  this->time = nullptr;
  this->memory = memory;
  SDL_SetMainReady();
  if(SDL_Init(SDL_INIT_VIDEO) != 0) {
    printf("could not init sdl: %s", SDL_GetError());
    exit(-1);
  }

  window = SDL_CreateWindow("gbcemu - alex midlash",
                            SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED,
                            2000, 2000,
                            0);
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}

IO::~IO() {
  SDL_Quit();
}

const uint16_t VBK_REG = 0xFF4F;

void IO::addTimeManager(TimeManager *time) {
  this->time = time;
}
void IO::drawAll() {
  bool debug_mode = true;
  if(time) {
    sprintf(title, "gbcemu - idle:(%d/16) - alex midlash", time->last_idle);
    SDL_SetWindowTitle(window, title);
  }
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  if(debug_mode) {
    drawBank0();
    drawBank1();
    drawScene();
  } else {
    if(memory->read8(VBK_REG) & 1) {
      drawBank1();
    } else {
      drawBank0();
    }
  }
  SDL_RenderPresent(renderer);
}

void IO::drawScene() {
  int startx = 256;
  int starty = 0;
  for(int entry=0;entry<40;entry++) {
    int start = (OAM_START + entry*4);
    uint8_t y = memory->read8(start + 0);
    uint8_t x = memory->read8(start + 1);
    uint8_t tile = memory->read8(start + 2);
    uint8_t attr = memory->read8(start + 3);
    if(y > 0 && y < 160 && x >0 && x < 168) {
      printf("drawing oam %d\n", entry);
      drawTile(tile, startx + x, starty + y);
    } else {
      printf("invisible oam: %d\n", entry);
    }
  }
   
}

void IO::drawTile(int tilenum, int x, int y) {
  static const uint8_t colors[] = {
    0, 0, 255, 255,
    0, 255, 0, 255,
    0, 255, 255, 255,
    255,255,0, 255};
  uint16_t address = 0x8000 + tilenum*16;
  for(int i=0;i<16;i+=2) {
    uint8_t lower = memory->read8(address+i);
    uint8_t upper = memory->read8(address+1+i);
    for(int xi=0;xi<8;xi++) {
      uint8_t val = select_bit(lower, 7-xi) + (select_bit(upper, 7-xi)<<1);
      /*printf("tile %d at (%d,%d)=%d\n", 
             tilenum,
             xi,i/2,
             val);*/
      SDL_SetRenderDrawColor(renderer,
                             colors[val*4], 
                             colors[val*4+1], 
                             colors[val*4+2],
                             colors[val*4+3]);
      SDL_RenderDrawPoint(renderer, x+xi, y+(i/2));
    }
  }
}
void IO::drawBank0() {
  puts("drawBank0 called");
  for(int tile=0;tile<384;tile++) {
    drawTile(tile, CBANK_0X+8*(tile%16), CBANK_0X+(tile/16)*8);
  }
}
void IO::drawBank1() {
  puts("drawBank1 called");
  /*
  for(int tile=0;tile<192;tile++) {
    drawTile(tile, CBANK_0X+8*(tile%8), CBANK_1Y+(tile/8)*8);
  }*/
}

// return whether or not the app should continue
bool IO::update() {
  SDL_Event ev;
  while(SDL_PollEvent(&ev)) {
    switch(ev.type) {
      case SDL_KEYUP:
        if(ev.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
          return false;
        break;
    }
  }
  return true;
}
