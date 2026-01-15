CC      := cc
CFLAGS  := -Wall -Wextra -Iinclude
SRC     := $(wildcard src/*.c)
BIN     := build/$(notdir $(CURDIR))

GLFW    := $(shell pkg-config --libs glfw3)
OPENGL  := -framework OpenGL
CFLAGS += $(shell pkg-config --cflags glfw3)
CFLAGS += -I/usr/include/glm

$(BIN): $(SRC)
	@mkdir -p build
	$(CC) $(CFLAGS) $^ -o $@ $(GLFW) $(OPENGL)

.PHONY: clean
clean:
	rm -rf build

