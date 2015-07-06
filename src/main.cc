#include "cpu.h"
#include "rom.h"

int main() {
  Rom bios("../roms/LoZ.gbc");
  CPU cpu(bios);

  for(int i=0;i<100;i++) {
    cpu.step();
  }
  return 0;
}





