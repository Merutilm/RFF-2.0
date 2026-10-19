#!/bin/bash

set -euo pipefail
cd "$(dirname "$0")" || exit 1

ROOT_DIR="$PWD"
EXPORT_DIR="$ROOT_DIR/export"

mkdir -p "$EXPORT_DIR"

# Linux x86-64
cmake -S "$ROOT_DIR" \
        -B "$EXPORT_DIR/build-linux" -G Ninja \
        -DCMAKE_TOOLCHAIN_FILE="$ROOT_DIR/cmake/linux-x86_64.cmake" \
        -DRFF_RELEASE=ON \
        -DCMAKE_RUNTIME_OUTPUT_DIRECTORY="$ROOT_DIR/bin"
cmake --build "$EXPORT_DIR/build-linux"


## Windows x86-64
cmake -S "$ROOT_DIR" \
      -B "$EXPORT_DIR/build-windows" -G Ninja \
      -DCMAKE_TOOLCHAIN_FILE="$ROOT_DIR/cmake/windows-x86_64.cmake" \
      -DMSYS2_CLANG="/opt/mingw" \
      -DRFF_RELEASE=ON \
      -DVulkan_INCLUDE_DIR=/opt/mingw/include \
      -Dglfw3_DIR=/opt/mingw/lib/cmake/glfw3 \
      -DVulkan_LIBRARY=/opt/mingw/lib/libvulkan-1.dll.a \
      -DCMAKE_RUNTIME_OUTPUT_DIRECTORY="$ROOT_DIR/bin"

cmake --build "$EXPORT_DIR/build-windows"

# ZIP packages


cp bin/put_all_dll_to_bin/* bin
zip -r -FS RFF-windows.zip bin/RFF.exe bin/*.dll res shaders/out
zip -r -FS RFF-linux.zip bin/RFF res shaders/out
rm bin/*.dll

echo "Build and packaging completed."