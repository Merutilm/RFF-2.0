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

sudo cp /opt/mingw/bin/libopencv_core4110.dll bin
sudo cp /opt/mingw/bin/libopencv_highgui4110.dll bin
sudo cp /opt/mingw/bin/libopencv_imgcodecs4110.dll bin
sudo cp /opt/mingw/bin/libopencv_imgproc4110.dll bin
sudo cp /opt/mingw/bin/libopencv_videoio4110.dll bin

zip -r -FS RFF-windows.zip bin/RFF.exe bin/*.dll res shaders/out
zip -r -FS RFF-linux.zip bin/RFF res shaders/out

sudo rm bin/*.dll

echo "Build and packaging completed."