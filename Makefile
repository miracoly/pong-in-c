###
CFLAGS  = -std=c17
CFLAGS += -g
CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -pedantic
CFLAGS += -Werror
CFLAGS += -Wmissing-declarations
CFLAGS += -DUNITY_SUPPORT_64 -DUNITY_OUTPUT_COLOR
CFLAGS += -I$(NIX_CFLAGS_COMPILE)
CFLAGS += -lSDL2

ASANFLAGS  = -fsanitize=address
ASANFLAGS += -fno-common
ASANFLAGS += -fno-omit-frame-pointer

.PHONY: all
all: build
	@./game.out

build: ./src/*.c
	@echo Compiling $@
	@$(CC) $(CFLAGS) ./src/*.c -o game.out

.PHONY: clean
clean:
	rm -rf *.o *.out *.out.dSYM
