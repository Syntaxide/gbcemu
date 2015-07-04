#include "cpu.h"
#include "rom.h"

int main() {
  Rom bios("../roms/gbc_bios.bin", false);
  CPU cpu(bios);

  for(int i=0;i<100;i++) {
    cpu.step();
  }
  return 0;
}





