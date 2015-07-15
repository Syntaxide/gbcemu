#include "io.h"
#include "SDL2/SDL.h"

IO::IO(Memory *memory) {
  this->memory = memory;
  SDL_SetMainReady();
  if(SDL_Init(SDL_INIT_VIDEO) != 0) {
    printf("could not init sdl: %s", SDL_GetError());
    exit(-1);
  }

  SDL_CreateWindow("gbcemu - alex midlash",
                   SDL_WINDOWPOS_CENTERED,
                   SDL_WINDOWPOS_CENTERED,
                   160, 144,
                   0);
}

IO::~IO() {
  SDL_Quit();
}

void IO::drawAll() {
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
