LIBS = -lmingw32 -lm -lSDL2main -lSDL2

###
CC = gcc
CFLAGS  = -std=c99
CFLAGS += -g
CFLAGS += -Wall
CFLAGS += -pedantic
CFLAGS += -Werror
CFLAGS += -Wmissing-declarations
CFLAGS += -DUNITY_SUPPORT_64

ASANFLAGS  = -fsanitize=address
ASANFLAGS += -fno-common
ASANFLAGS += -fno-omit-frame-pointer

INCLUDES = -I "lib\SDL2-2.0.12\i686-w64-mingw32\include"
LINKING = -L "lib\SDL2-2.0.12\i686-w64-mingw32\lib"

run:
	@echo [ INFO ] Compiling... 
	@$(CC) $(CFLAGS) src/*.c $(INCLUDES) $(LINKING) $(LIBS) -o build/chip8.out
	@./build/chip8.out

.PHONY: clean
clean:
	rm -rf *.o *.out *.out.dSYM