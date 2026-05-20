CC      := cc
CFLAGS  := -Wall -Wextra -Iinclude

SRC     := $(wildcard src/*.c)
BIN     := build/$(notdir $(CURDIR))

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

.PHONY: clean
clean:
	rm -rf build

