CC      := cc
CFLAGS  := -Wall -Wextra -Iinclude

SRC     := $(wildcard src/*.c)
BIN     := build/$(notdir $(CURDIR))
ASAN_BIN := build/$(notdir $(CURDIR))-asan

# Detect OS
UNAME_S := $(shell uname -s)

# Common libraries
SDL3    := $(shell pkg-config --libs sdl3)
CGLM    := $(shell pkg-config --libs cglm)

# Common flags
CFLAGS += $(shell pkg-config --cflags sdl3)
CFLAGS += $(shell pkg-config --cflags cglm) 

# Platform-specific linker flags
ifeq ($(UNAME_S),Darwin)
    OPENGL := -framework OpenGL
    SYS_LIBS :=
endif

ifeq ($(UNAME_S),Linux)
    OPENGL := -lGL
    SYS_LIBS := -lm
endif


$(BIN): $(SRC)
	@mkdir -p build
	$(CC) $(CFLAGS) $^ -o $@ $(SDL3) $(OPENGL) $(CGLM) $(SYS_LIBS)

.PHONY: asan
asan: CFLAGS += -fsanitize=address -g -O0 -fno-omit-frame-pointer
asan: $(SRC)
	@mkdir -p build
	$(CC) $(CFLAGS) $^ -o $(ASAN_BIN) $(SDL3) $(OPENGL) $(CGLM) $(SYS_LIBS)

.PHONY: clean
clean:
	rm -rf build

