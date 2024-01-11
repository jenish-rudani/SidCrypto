# Makefile for building 'sid.c' for multiple platforms

# Default compiler settings
CFLAGS := -Wall -Wextra -std=c99

# Executable name
EXE_NAME := sid

# Object file
OBJ := $(EXE_NAME).o

# Compilers for specific platforms
CC_LINUX := x86_64-linux-gnu-gcc
CC_DARWIN := clang
CC_WINDOWS := x86_64-w64-mingw32-gcc

# Build directories for each platform
BUILD_DIR_LINUX := build/linux
BUILD_DIR_DARWIN := build/darwin
BUILD_DIR_WINDOWS := build/windows

# Phony targets for each platform
.PHONY: compile-linux compile-darwin compile-windows clean

compile-linux: clean
	$(MAKE) CC=$(CC_LINUX) BUILD_DIR=$(BUILD_DIR_LINUX) EXE=$(BUILD_DIR_LINUX)/$(EXE_NAME) $(BUILD_DIR_LINUX)/$(EXE_NAME)
	rm -f $(OBJ)

compile-darwin: clean
	$(MAKE) CC=$(CC_DARWIN) BUILD_DIR=$(BUILD_DIR_DARWIN) EXE=$(BUILD_DIR_DARWIN)/$(EXE_NAME) $(BUILD_DIR_DARWIN)/$(EXE_NAME)
	rm -f $(OBJ)

compile-windows: clean
	$(MAKE) CC=$(CC_WINDOWS) BUILD_DIR=$(BUILD_DIR_WINDOWS) EXE=$(BUILD_DIR_WINDOWS)/$(EXE_NAME).exe $(BUILD_DIR_WINDOWS)/$(EXE_NAME).exe
	rm -f $(OBJ)

# Ensure build directory exists
$(BUILD_DIR_LINUX) $(BUILD_DIR_DARWIN) $(BUILD_DIR_WINDOWS):
	mkdir -p $@

# Compile the program
$(BUILD_DIR_LINUX)/$(EXE_NAME) $(BUILD_DIR_DARWIN)/$(EXE_NAME) $(BUILD_DIR_WINDOWS)/$(EXE_NAME).exe: $(OBJ) | $(BUILD_DIR_LINUX) $(BUILD_DIR_DARWIN) $(BUILD_DIR_WINDOWS)
	$(CC) $(CFLAGS) $(OBJ) -o $@

# Compile the object file
$(OBJ): sid.c
	$(CC) $(CFLAGS) -c sid.c

# Clean up
clean:
	rm -f $(OBJ)
	rm -rf build

# Default target
all: compile-darwin compile-windows compile-linux
	@echo "Comping All Platforms"
