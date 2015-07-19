#!/usr/bin/python
with open("vram.txt", "r") as vram:
  with open("vram.bin", "w") as out:
    text = vram.read().rstrip()
    while(len(text) > 0):
      two = text[:2]
      text = text[2:]
      out.write(chr(int("0x"+two, 16)))
