#include "rom.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


void slurp(const char *filename, uint8_t **out, long *filesize) {
  FILE *file = fopen(filename, "rb");
  if(!file) {
    puts("could not open file");
    exit(-1);
  }
  fseek(file, 0, SEEK_END);

  long len = 0;
  len = ftell(file);
  if(filesize != 0) {
    *filesize = len;
  }
  
  rewind(file);
  *out = new uint8_t[len];
  if(out == 0) {
    printf("unable to allocate memory for file:%s\n", filename);
    exit(-1);
  }

  for(long int i=0;i<len;i++) {
    (*out)[i] = fgetc(file);
  }
}
  

Rom::Rom(const char *filename, bool cartridge) {
  slurp(filename, &buffer,  &filesize);
  printf("read %ld bytes from %s\n", filesize, filename);

  if(cartridge)
    showInfo();
}

Rom::~Rom() {
  delete buffer;
}

void Rom::showInfo() {
  printf("name: ");
  printString(0x0134, 0x0142);

  printf("alt start: %x%x\n", readByte(0x103), readByte(0x102));
  printf("system: %x\n", readByte(0x143));
  printf("cart type: %x\n", readByte(0x147));
}


uint8_t Rom::readByte(long index) const {
  if(index >= 0 && index < filesize) {
    return buffer[index];
  } else {
    printf("out of bounds read! %ld", index);
    exit(-1);
  }
}

void Rom::printString(long start, long end) {
  for(long i=start;i > 0 && i<end && i < filesize;i++) {
    putchar(buffer[i]);
    if(buffer[i] == 0) {
      printf("\n");
      return;
    }
  }
}

long Rom::getLength() {
  return filesize;
}

/*
   short view into rom
   used for instruction decoding
 */
RomView::RomView(const Rom &rom, int start) : mRom(rom), mStart(start) {
}

uint8_t RomView::operator[](long index) const {
  if(index < 3 && index >= 0)
    return mRom.readByte(index + mStart);
  else {
    printf("bad index to romview: %ld\n", index);
    exit(-1);
  }
}

