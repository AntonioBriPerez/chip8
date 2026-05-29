# CHIP-8 Emulator

A CHIP-8 emulator written in C (C99) with SDL2.

## What is CHIP-8?

CHIP-8 is an interpreted programming language from the mid-1970s, originally designed for 8-bit microcomputers. It became popular as a platform for simple games like Pong, Space Invaders, and Tetris. Emulating it is a classic entry point into emulator development.

## Features

- Full CHIP-8 instruction set
- 64×32 pixel display rendered via SDL2
- Hexadecimal keypad mapped to keyboard
- Delay and sound timers at 60 Hz
- Beep on sound timer

## Stack

- **Language**: C99
- **Display / Input / Audio**: SDL2
- **Build**: Makefile

## Usage

```sh
make
./chip8 roms/ibm_logo.ch8
```

## Keyboard layout

```
CHIP-8      Keyboard
1 2 3 C  →  1 2 3 4
4 5 6 D  →  Q W E R
7 8 9 E  →  A S D F
A 0 B F  →  Z X C V
```

## References

- [Cowgod's CHIP-8 Technical Reference](http://devernay.free.fr/hacks/chip8/C8TECH10.HTM)
- [Guide to making a CHIP-8 emulator — Tobias V. Langhoff](https://tobiasvl.github.io/blog/write-a-chip-8-emulator/)
- [CHIP-8 ROMs](https://github.com/kripod/chip8-roms)
