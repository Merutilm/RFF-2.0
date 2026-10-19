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
      -DRFF_RELEASE=ON \
      -DCMAKE_RUNTIME_OUTPUT_DIRECTORY="$ROOT_DIR/bin"

cmake --build "$EXPORT_DIR/build-windows"

# ZIP packages

zip -r -FS RFF-windows.zip bin/RFF.exe res shaders/out
zip -r -FS RFF-linux.zip bin/RFF res shaders/out

echo "Build and packaging completed."