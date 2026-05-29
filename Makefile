CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99 $(shell sdl2-config --cflags)
LDFLAGS = $(shell sdl2-config --libs)

TARGET = chip8
SRCS = $(wildcard src/*.c) # recoge todos los archivos .c en el directorio src
OBJS = $(SRCS:.c=.o) # reemplaza la extensión .c por .o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.c # para cualquier .o compilalo a partir de su correspondiente .c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET) $(ROM)