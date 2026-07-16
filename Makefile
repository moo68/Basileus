CC      := cc
CFLAGS  := -Wall -Wextra -Iinclude

SRC     := $(wildcard src/*.c)
BIN     := build/$(notdir $(CURDIR))
ASAN_BIN := build/$(notdir $(CURDIR))-asan

# Detect OS
UNAME_S := $(shell uname -s)

# Common libraries
GLFW    := $(shell pkg-config --libs glfw3)
CGLM    := $(shell pkg-config --libs cglm)

# Common flags
CFLAGS += $(shell pkg-config --cflags glfw3)
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
	$(CC) $(CFLAGS) $^ -o $@ $(GLFW) $(OPENGL) $(CGLM) $(SYS_LIBS)

.PHONY: asan
asan: CFLAGS += -fsanitize=address -g -O0 -fno-omit-frame-pointer
asan: $(SRC)
	@mkdir -p build
	$(CC) $(CFLAGS) $^ -o $(ASAN_BIN) $(GLFW) $(OPENGL) $(CGLM) $(SYS_LIBS)

.PHONY: clean
clean:
	rm -rf build

