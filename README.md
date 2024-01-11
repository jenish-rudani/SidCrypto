# SID CRYPTO

## Description

Convert SID to MAC and viceversa.

## Supported Systems

This project can be compiled on the following platforms:
- Linux
- macOS (Darwin)
- Windows

## Prerequisites

Before you begin, ensure you have met the following requirements:
- For Building for Linux on arm64 MAC, ensure `x86_64-linux-gnu-gcc` is installed on m1 MAC. [Check this](https://github.com/messense/homebrew-macos-cross-toolchains)
- For Building for Windows on arm64 MAC, ensure `x86_64-w64-mingw32-gcc` is installed. 
 `brew install mingw-w64`

## Compiling the Program

To compile the program, use the following commands based on your platform:

### For Linux
```
make compile-linux
```

### For macOS
```
make compile-darwin
```

### For Windows
```
make compile-windows
```

## Cleaning Build Files

To clean up the build files and remove any compiled binaries and object files, run:
```
make clean
```

## Additional Information

- If you're running on a platform different from the above-mentioned, you may need to modify the Makefile to support your specific compiler and system.
- The Makefile is designed to place the built binaries in a platform-specific directory under `build/`.

## Contributing to the Project

If you have suggestions for improving the project, please open an issue or submit a pull request.