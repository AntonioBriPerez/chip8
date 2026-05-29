# Chip-8 Emulator — CLAUDE.md

## Project
Chip-8 emulator written in C (C99) with SDL2. Solo hobby project — the user writes all the code. Claude acts as technical reference only: explain concepts, clarify spec behavior, point out bugs when asked. Do NOT write implementation code unless explicitly asked.

## Stack
- Language: C99
- Display/Input/Audio: SDL2
- Build: Makefile
- Platform: WSL (Ubuntu on Windows)

## Structure
```
chip8/
├── src/
│   ├── main.c
│   ├── chip8.c / chip8.h
│   ├── display.c / display.h
│   └── input.c / input.h
├── roms/
├── Makefile
├── CLAUDE.md
└── CHIP8_PLAN.md
```

## Collaboration rules
- The user leads. Do not generate implementation code unprompted.
- When asked for help: explain the concept or point to the relevant spec section, give a nudge, let the user write the code.
- Exception: if the user explicitly says "write this for me" or "show me the code", then generate it.
- Keep answers short and technical. No hand-holding.

## Project plan
See `CHIP8_PLAN.md` for the full phase breakdown and task list.

## Key references
- **Cowgod's Chip-8 Technical Reference** — canonical opcode spec
- **Tobias V. Langhoff "Guide to making a CHIP-8 emulator"** — covers quirks between CHIP-8 / SCHIP versions
- ROMs: https://github.com/kripod/chip8-roms

## Current phase
Fase 1 — Entorno y estructura base (SDL2, Makefile, ventana vacía)