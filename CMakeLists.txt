cmake_minimum_required(VERSION 3.20)

# Mirrors `BIN := build/$(notdir $(CURDIR))` — binary named after the folder
get_filename_component(PROJECT_DIR_NAME "${CMAKE_SOURCE_DIR}" NAME)
project(${PROJECT_DIR_NAME} C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Without this, no per-configuration flags get applied at all — you'd get
# neither optimization nor debug symbols, just the bare compiler default.
# Only relevant for single-config generators (Makefiles, Ninja); multi-config
# generators like Xcode/Visual Studio pick the config at build time instead
# via `cmake --build . --config Release` and ignore this variable.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()

# CMake's built-in defaults for GCC/Clang are already sensible:
#   Debug:   -g
#   Release: -O3 -DNDEBUG   (NDEBUG strips any assert() calls)
# Uncomment to override if you want something specific, e.g. -O2 instead:
# set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")

# Put the binary in build/ directly, matching the old layout
# (with multi-config generators like Xcode/MSVC this still lands under build/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

# --- dependencies ---
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL3 REQUIRED IMPORTED_TARGET sdl3)

# cglm is header-only unless you use the precompiled glmc_ API (cglm/call.h).
# We deliberately do NOT link against libcglm.so: the distro-packaged .so
# has a known bug where it isn't linked against libm itself, causing an
# IFUNC crash on `sinf` at runtime. Pulling in only the headers sidesteps
# the broken shared library entirely. If you ever need cglm/call.h, this
# will need to change.
pkg_check_modules(CGLM REQUIRED cglm)

# find_package(OpenGL) resolves to -framework OpenGL on macOS and -lGL on
# Linux automatically — this replaces the manual UNAME_S branching entirely.
find_package(OpenGL REQUIRED)

# --- sources ---
# NOTE: glob matches `SRC := $(wildcard src/*.c)`, but CMake won't notice new
# files added later without a re-run of `cmake ..`. Listing sources explicitly
# is more robust long-term; switch to that if it bites you.
file(GLOB SRC CONFIGURE_DEPENDS "src/*.c")

# --- main target ---
add_executable(${PROJECT_NAME} ${SRC})
target_include_directories(${PROJECT_NAME} PRIVATE include ${CGLM_INCLUDE_DIRS})
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
target_link_libraries(${PROJECT_NAME} PRIVATE
    PkgConfig::SDL3
    OpenGL::GL
)
if(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE m)
endif()

# Link-time optimization for Release builds only — lets the compiler
# optimize across translation units, which -O3 alone doesn't do.
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    include(CheckIPOSupported)
    check_ipo_supported(RESULT lto_supported OUTPUT lto_error)
    if(lto_supported)
        set_target_properties(${PROJECT_NAME} PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
    else()
        message(STATUS "LTO not supported: ${lto_error}")
    endif()
endif()

# --- asan target (equivalent of `make asan`) ---
# Target name is just "asan" so you can run: cmake --build . --target asan
# The output binary itself is still named e.g. build/Basileus-asan.
add_executable(asan EXCLUDE_FROM_ALL ${SRC})
set_target_properties(asan PROPERTIES OUTPUT_NAME "${PROJECT_NAME}-asan")
target_include_directories(asan PRIVATE include ${CGLM_INCLUDE_DIRS})
target_compile_options(asan PRIVATE
    -Wall -Wextra -fsanitize=address -g -O0 -fno-omit-frame-pointer
)
target_link_options(asan PRIVATE -fsanitize=address)
target_link_libraries(asan PRIVATE
    PkgConfig::SDL3
    OpenGL::GL
)
if(UNIX AND NOT APPLE)
    target_link_libraries(asan PRIVATE m)
endif()
