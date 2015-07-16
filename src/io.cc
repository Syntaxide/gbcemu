#include "io.h"
#include "SDL2/SDL.h"

IO::IO(Memory *memory) {
  this->memory = memory;
  SDL_SetMainReady();
  if(SDL_Init(SDL_INIT_VIDEO) != 0) {
    printf("could not init sdl: %s", SDL_GetError());
    exit(-1);
  }

  window = SDL_CreateWindow("gbcemu - alex midlash",
                            SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED,
                            256, 256,
                            0);
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}

IO::~IO() {
  SDL_Quit();
}

const uint16_t VBK_REG = 0xFF4F;

void IO::drawAll() {
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);

  if(memory->read8(VBK_REG) & 1) {
    drawBank1();
  } else {
    drawBank0();
  }
}

void IO::drawBank0() {
  puts("drawBank0 called");
}
void IO::drawBank1() {
  puts("drawBank1 called");
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
