# Chip-8 Emulator in C — Project Plan

## Stack
- **Language**: C (C99)
- **Display/Input/Audio**: SDL2
- **Build**: Makefile

---

## Fase 1 — Entorno y estructura base

### Objetivos
Tener un proyecto que compile y abra una ventana vacía.

### Tareas
- [ ] Instalar SDL2 (headers + lib) y verificar que el compilador lo encuentra
- [ ] Crear estructura de carpetas:
  ```
  chip8/
  ├── src/
  │   ├── main.c
  │   ├── chip8.c
  │   ├── chip8.h
  │   ├── display.c
  │   ├── display.h
  │   ├── input.c
  │   └── input.h
  ├── roms/
  └── Makefile
  ```
- [ ] Escribir el `Makefile` con flags: `-Wall -Wextra -std=c99 $(sdl2-config --cflags --libs)`
- [ ] En `main.c`: inicializar SDL2, abrir ventana 640x320 (escala x10 del display 64x32), bucle principal vacío con `SDL_PollEvent`, cerrar limpiamente con `SDL_QUIT`
- [ ] Verificar: la ventana abre, se cierra con la X, no hay warnings

### Conceptos clave
- `SDL_Init`, `SDL_CreateWindow`, `SDL_CreateRenderer`
- Bucle de eventos con `SDL_PollEvent`

---

## Fase 2 — Estructura del Chip-8

### Objetivos
Definir en código toda la "máquina" Chip-8 y cargar una ROM.

### Tareas
- [ ] En `chip8.h` definir el struct `Chip8`:
  ```c
  typedef struct {
      uint8_t  memory[4096];
      uint8_t  V[16];          // registros V0-VF
      uint16_t I;              // registro índice
      uint16_t pc;             // program counter
      uint8_t  sp;             // stack pointer
      uint16_t stack[16];
      uint8_t  delay_timer;
      uint8_t  sound_timer;
      uint8_t  keypad[16];
      uint8_t  display[64*32]; // 1 bit por pixel
      uint8_t  draw_flag;
  } Chip8;
  ```
- [ ] Función `chip8_init(Chip8*)` — poner pc a 0x200, limpiar todo
- [ ] Cargar la **fontset** en memoria (posiciones 0x000–0x04F): 16 caracteres × 5 bytes cada uno. Búscala como "chip8 fontset" — es estándar.
- [ ] Función `chip8_load_rom(Chip8*, const char* path)` — abrir fichero binario, leer bytes a `memory[0x200]` en adelante
- [ ] En `main.c`: recibir path de ROM como argumento `argv[1]`, inicializar y cargar
- [ ] Verificar: compilar sin errores, imprimir los primeros bytes de memoria como hex para confirmar que la ROM cargó

### Conceptos clave
- El programa empieza en `0x000–0x1FF` reservado (intérprete + fonts)
- Las ROMs se cargan desde `0x200`
- Cada opcode son **2 bytes** big-endian

---

## Fase 3 — Ciclo de emulación y primeros opcodes

### Objetivos
Implementar el bucle fetch→decode→execute y los opcodes más simples.

### Tareas
- [ ] Función `chip8_cycle(Chip8*)`:
  1. **Fetch**: `opcode = memory[pc] << 8 | memory[pc+1]`
  2. **Decode + Execute**: `switch(opcode & 0xF000)`
  3. Incrementar `pc += 2` (salvo saltos)
- [ ] Implementar estos opcodes primero (los más simples):
  - `00E0` — CLS: limpiar display
  - `1NNN` — JP addr: saltar a NNN
  - `6XNN` — LD Vx, byte: cargar NN en Vx
  - `7XNN` — ADD Vx, byte: Vx += NN
  - `ANNN` — LD I, addr: I = NNN
  - `00EE` — RET: return de subrutina
  - `2NNN` — CALL addr: llamar subrutina
- [ ] Añadir un `printf` temporal de debug que imprima el opcode en cada ciclo
- [ ] Verificar con una ROM simple (IBM Logo es la más usada para esto — búscala como "chip8 IBM logo rom")

### Conceptos clave
- Los 4 bits altos (`opcode & 0xF000`) identifican la familia
- `X = (opcode & 0x0F00) >> 8` — índice de registro Vx
- `Y = (opcode & 0x00F0) >> 4` — índice de registro Vy
- `N = opcode & 0x000F` — nibble
- `NN = opcode & 0x00FF` — byte
- `NNN = opcode & 0x0FFF` — dirección

---

## Fase 4 — Display

### Objetivos
Renderizar el framebuffer del Chip-8 en la ventana SDL2.

### Tareas
- [ ] Implementar el opcode `DXYN` — DRAW:
  - Lee N bytes desde `memory[I]`
  - Los dibuja en pantalla en posición (Vx, Vy)
  - Cada bit del byte es un pixel (XOR con el pixel existente)
  - Si algún pixel pasa de 1→0, poner `VF = 1` (detección de colisión)
  - Activar `draw_flag`
- [ ] En `display.c`:
  - Función `display_render(SDL_Renderer*, uint8_t display[64*32])`
  - Iterar el array, por cada pixel activo dibujar un rect de 10×10 px blanco
- [ ] En el bucle principal: si `draw_flag == 1`, llamar a render y bajar el flag
- [ ] Verificar: la ROM IBM Logo debe dibujar el logo de IBM correctamente

### Conceptos clave
- El display es 64 wide × 32 tall
- Los sprites se envuelven (wrap) si se salen de pantalla
- XOR es clave: si dibujas encima de un pixel activo, lo apagas

---

## Fase 5 — Opcodes restantes (lógica y aritmética)

### Objetivos
Completar todos los opcodes de lógica, aritmética y saltos condicionales.

### Tareas
- [ ] Implementar el bloque `8XYN` (operaciones entre registros):
  - `8XY0` LD, `8XY1` OR, `8XY2` AND, `8XY3` XOR
  - `8XY4` ADD (con carry en VF)
  - `8XY5` SUB (con borrow en VF)
  - `8XY6` SHR, `8XYE` SHL (cuidado: hay quirks entre versiones)
  - `8XY7` SUBN
- [ ] Implementar saltos condicionales:
  - `3XNN` — SE Vx, byte (skip if equal)
  - `4XNN` — SNE Vx, byte
  - `5XY0` — SE Vx, Vy
  - `9XY0` — SNE Vx, Vy
- [ ] Implementar misc:
  - `CXNN` — RND: Vx = random & NN
  - `FX07` — LD Vx, DT (leer delay timer)
  - `FX15` — LD DT, Vx (escribir delay timer)
  - `FX18` — LD ST, Vx (escribir sound timer)
  - `FX1E` — ADD I, Vx
  - `FX29` — LD F, Vx (I apunta al sprite del dígito Vx)
  - `FX33` — LD B, Vx (BCD de Vx en memory[I], I+1, I+2)
  - `FX55` — LD [I], Vx (volcar registros a memoria)
  - `FX65` — LD Vx, [I] (cargar registros desde memoria)

### Conceptos clave
- `rand() % 256` para RND
- BCD: separar centenas, decenas, unidades de un número 0–255

---

## Fase 6 — Input

### Objetivos
Mapear el teclado físico al keypad hexadecimal del Chip-8.

### Tareas
- [ ] Definir el mapping (estándar):
  ```
  Chip-8    →   Teclado
  1 2 3 C       1 2 3 4
  4 5 6 D       Q W E R
  7 8 9 E       A S D F
  A 0 B F       Z X C V
  ```
- [ ] En `input.c`: función `input_update(SDL_Event*, uint8_t keypad[16])`
- [ ] Implementar opcodes de input:
  - `EX9E` — SKP Vx (skip si tecla Vx pulsada)
  - `EXA1` — SKNP Vx (skip si tecla Vx NO pulsada)
  - `FX0A` — LD Vx, K (bloquear hasta que se pulse una tecla)
- [ ] Verificar con la ROM PONG o TETRIS — deben responder al input

---

## Fase 7 — Timers y audio

### Objetivos
Sincronizar los timers a 60 Hz y emitir un pitido con el sound timer.

### Tareas
- [ ] Decrementar `delay_timer` y `sound_timer` a 60 Hz (no por cada ciclo)
  - Estrategia: ejecutar ~8-10 ciclos de CPU por cada tick de 60 Hz
  - Usar `SDL_GetTicks()` o `SDL_Delay(16)` para limitar a 60 FPS
- [ ] Audio: cuando `sound_timer > 0`, emitir un beep
  - Opción simple: `SDL_OpenAudioDevice` con una onda sinusoidal o cuadrada generada en un callback
  - Opción más simple aún: `printf("\a")` para el terminal (vale para empezar)
- [ ] Verificar: los juegos que usan timers (Pong, Space Invaders) deben correr a velocidad correcta

---

## Fase 8 — Pulir y jugar

### Objetivos
Que el emulador sea usable de verdad.

### Tareas
- [ ] Probar ROMs variadas: Pong, Space Invaders, Tetris, Breakout
- [ ] Añadir pausa con tecla `P`
- [ ] Añadir reset con tecla `R` (recargar la ROM)
- [ ] Título de ventana con el nombre de la ROM
- [ ] Manejar ROMs con **quirks** si algún juego no funciona bien (hay diferencias entre versiones CHIP-8/SCHIP)
- [ ] (Opcional) Color configurable para los pixels
- [ ] (Opcional) Guardar/cargar estado

---

## ROMs para testing (por fase)

| ROM | Fase | Qué prueba |
|-----|------|------------|
| IBM Logo | 3–4 | Opcodes básicos + display |
| BC_test | 5 | Opcodes aritméticos |
| test_opcode | 5 | Suite completa de opcodes |
| Pong | 6–7 | Input + timers |
| Space Invaders | 7–8 | Todo junto |

Búscalas en: **https://github.com/kripod/chip8-roms**

---

## Referencia rápida

- Especificación completa: **Cowgod's Chip-8 Technical Reference** (búscala así en Google, es la referencia canónica)
- Guía de opcodes con quirks: **Tobias V. Langhoff "Guide to making a CHIP-8 emulator"**
- Para cada opcode que implementes, consulta la spec antes de escribir el código

---

## Cómo pedir ayuda

Cuando te atasques, ven con:
1. La fase/tarea en la que estás
2. El opcode o comportamiento concreto que no funciona
3. Tu código actual y el output/error que obtienes

Así puedo darte la pista justa sin spoilearte la solución.