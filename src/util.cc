#include "util.h"
#include <stdio.h>
#include <stdlib.h>
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
