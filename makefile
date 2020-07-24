UNAME := $(shell uname)
ifeq ($(UNAME), Darwin) 
	LIBS = -lm -lSDL2main -lSDL2
else
	LIBS = -lmingw32 -lm -lSDL2main -lSDL2
endif

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

ifeq (run,$(firstword $(MAKECMDGOALS)))
  # use the rest as arguments for "run"
  RUN_ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
  # ...and turn them into do-nothing targets
  $(eval $(RUN_ARGS):;@:)
endif

make:
	@echo [ INFO ] Compiling...
	@echo [ INFO ] Compilation DONE 
	@$(CC) $(CFLAGS) src/*.c $(INCLUDES) $(LINKING) $(LIBS) -o build/chip8.out

run:
	@./build/chip8.out $(RUN_ARGS)

.PHONY: clean
clean:
	rm -rf *.o *.out *.out.dSYM