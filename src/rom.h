#pragma once
#include <stdint.h>

class Rom {
  public:
    Rom(const char *filename, bool cartridge = true);
    ~Rom();

    void showInfo(); 
    uint8_t readByte(long index) const; 
    void printString(long start, long end); 
    long getLength(); 
  private:
    uint8_t *buffer;
    long filesize;
};

/*
   short view into rom
   used for instruction decoding
 */
class RomViewBase {
  public:
    virtual uint8_t operator[](long index) const = 0;
};

class RomView : public RomViewBase{
  public:
    RomView(const Rom &rom, int start); 
    uint8_t operator[](long index) const override;
  private:
    const Rom &mRom;
    int mStart;
};
