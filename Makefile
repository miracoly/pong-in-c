###
CFLAGS  = -std=c17
CFLAGS += -g
CFLAGS += -Wall
CFLAGS += -Wextra
CFLAGS += -pedantic
CFLAGS += -Werror
CFLAGS += -Wmissing-declarations
CFLAGS += -DUNITY_SUPPORT_64 -DUNITY_OUTPUT_COLOR
CFLAGS += -lSDL2
CFLAGS += -lSDL2_ttf
CFLAGS += -lm

ASANFLAGS  = -fsanitize=address
ASANFLAGS += -fno-common
ASANFLAGS += -fno-omit-frame-pointer

.PHONY: all
all: build
	@./game.out

build: ./src/*.c
	@echo Compiling $@
	@$(CC) $(CFLAGS) ./src/main.c ./src/pong.c -o game.out

.PHONY: clean
clean:
	rm -rf *.o *.out *.out.dSYM
