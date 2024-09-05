# Makefile for building 'sidToMac.c' and 'macToSid.c' for multiple platforms

# Default compiler settings
CFLAGS := -Wall -Wextra -std=c99 -O0

# Executable names
EXE_NAME_SIDTOMAC := sidToMac
EXE_NAME_MACTOSID := macToSid

# Object files
OBJ_SIDTOMAC := $(EXE_NAME_SIDTOMAC).o
OBJ_MACTOSID := $(EXE_NAME_MACTOSID).o

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

# Linux
compile-linux: $(BUILD_DIR_LINUX)/$(EXE_NAME_SIDTOMAC) $(BUILD_DIR_LINUX)/$(EXE_NAME_MACTOSID)

# Darwin
compile-darwin: $(BUILD_DIR_DARWIN)/$(EXE_NAME_SIDTOMAC) $(BUILD_DIR_DARWIN)/$(EXE_NAME_MACTOSID)

# Windows
compile-windows: $(BUILD_DIR_WINDOWS)/$(EXE_NAME_SIDTOMAC).exe $(BUILD_DIR_WINDOWS)/$(EXE_NAME_MACTOSID).exe

# Compile sidToMac for all platforms
$(BUILD_DIR_LINUX)/$(EXE_NAME_SIDTOMAC) $(BUILD_DIR_DARWIN)/$(EXE_NAME_SIDTOMAC) $(BUILD_DIR_WINDOWS)/$(EXE_NAME_SIDTOMAC).exe: $(OBJ_SIDTOMAC) | $(BUILD_DIR_LINUX) $(BUILD_DIR_DARWIN) $(BUILD_DIR_WINDOWS)
	$(CC) $(CFLAGS) $(OBJ_SIDTOMAC) -o $@

# Compile macToSid for all platforms
$(BUILD_DIR_LINUX)/$(EXE_NAME_MACTOSID) $(BUILD_DIR_DARWIN)/$(EXE_NAME_MACTOSID) $(BUILD_DIR_WINDOWS)/$(EXE_NAME_MACTOSID).exe: $(OBJ_MACTOSID) | $(BUILD_DIR_LINUX) $(BUILD_DIR_DARWIN) $(BUILD_DIR_WINDOWS)
	$(CC) $(CFLAGS) $(OBJ_MACTOSID) -o $@

# Ensure build directory exists
$(BUILD_DIR_LINUX) $(BUILD_DIR_DARWIN) $(BUILD_DIR_WINDOWS):
	mkdir -p $@

# Compile the object files
$(OBJ_SIDTOMAC): sidToMac.c
	$(CC) $(CFLAGS) -c sidToMac.c

$(OBJ_MACTOSID): macToSid.c
	$(CC) $(CFLAGS) -c macToSid.c -o $(OBJ_MACTOSID)

# Clean up
clean:
	rm -f $(OBJ_SIDTOMAC) $(OBJ_MACTOSID)
	rm -rf build

# Default target
all: compile-darwin compile-windows compile-linux
	@echo "Compiling All Platforms"
