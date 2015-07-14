#include "cpu.h"
#include "rom.h"

int main(int argc, char **argv) {
  if(argc != 2) {
    puts("usage: ");
    puts("emu rom.gbc");
    return -1;
  } else {
    Rom bios(argv[1]);
    CPU cpu(bios);

    while(cpu.step()) {
    }
    return 0;
  }
}





