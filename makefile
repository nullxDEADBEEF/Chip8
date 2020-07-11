LIBS = -lm

###
CC = gcc
CFLAGS  = -std=c99
CFLAGS += -g
CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -pedantic
CFLAGS += -Werror
CFLAGS += -Wmissing-declarations
CFLAGS += -DUNITY_SUPPORT_64

ASANFLAGS  = -fsanitize=address
ASANFLAGS += -fno-common
ASANFLAGS += -fno-omit-frame-pointer

run:
	@echo [ INFO ] Compiling... 
	@$(CC) $(CFLAGS) src/*.c -o build/chip8.out $(LIBS)
	@./build/chip8.out

.PHONY: clean
clean:
	rm -rf *.o *.out *.out.dSYM