CC      := cc
CFLAGS  := -Wall -Wextra -Iinclude
SRC     := $(wildcard src/*.c)
BIN     := build/$(notdir $(CURDIR))

GLFW    := $(shell pkg-config --libs glfw3)
OPENGL  := -framework OpenGL
CGLM    := $(shell pkg-config --libs cglm)

CFLAGS += $(shell pkg-config --cflags glfw3)
CFLAGS += $(shell pkg-config --cflags cglm) 

$(BIN): $(SRC)
	@mkdir -p build
	$(CC) $(CFLAGS) $^ -o $@ $(GLFW) $(OPENGL) $(CGLM)

.PHONY: clean
clean:
	rm -rf build

