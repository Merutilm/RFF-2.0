# RFF 2.0


## What is RFF?

**RFF** is an abbreviation for <u>**Ridiculously Fast Fractal**</u>.

- As the name suggests, This program is designed to operate as quickly as possible rather than operating stably.

## Overview
### Important : This program is **NOT COMPATIBLE** with **RFF(Java)** file extensions!

- A program that achieves extremely fast `Power-2 MB2 set`.

- The application is built with the `Vulkan`.

- This program uses `Fast-period-guessing`*(a.k.a. FPG)* which I developed. It automatically generates the `longest period` of the selected location.
This value is unmodifiable.

- This program uses an `Multi-level Periodic Approximation` algorithm which I developed.
It completely replaces traditional `BLA`, achieving speedups of more than 2 times. \
To put it simply, it skips to the `Periodic point` directly.


- You can specify a compressor to render even extremely long period using less memory. \
Of course, the approximation table can also be compressed using this algorithm, and jumps a <u>**HUGE**</u> process! \
Therefore, If you are trying to render long periods (over `10,000,000` or so), You should compress the references. \
This will be <u>**SIGNIFICANTLY**</u> faster because it <u>**SUPERJUMPS**</u> process of table creation. 

- Save amazing images using shaders!

## Get Started

- It's very simple. Just go into the `releases`, 
download the zip,
unzip it and run it from the `bin` directory.

- Or you can build manually.

### Requirements
*IMPORTANT : To use this application, the GPU must support `Vulkan`.*

If the following error occurs even though `Vulkan` is installed correctly, it is caused by the GPU not supporting `Vulkan`. 
In this case, use version `2.0.12.1`. Supported OS of this version is only Windows.
```terminaloutput
- [INITIALIZE] No suitable physical device found 

- registry lookup failed to get layer manifest file 
```

### Launch Application
#### for Windows
If it does not run immediately after zip extraction, try the following steps.

1. Install [MSYS2](https://www.msys2.org/)
2. Launch `clang64.exe`
3. Update MSYS2 packages:
```bash
pacman -Syu
```

4. Restart `clang64.exe`

5. Install required packages:
```bash
pacman -S mingw-w64-clang-x86_64-gmp  # If you want to build gmp manually, do not run that command.
pacman -S mingw-w64-clang-x86_64-make
pacman -S mingw-w64-clang-x86_64-cmake
pacman -S mingw-w64-clang-x86_64-ninja
pacman -S mingw-w64-clang-x86_64-clang
pacman -S mingw-w64-clang-x86_64-opencv
pacman -S mingw-w64-clang-x86_64-vulkan
pacman -S mingw-w64-clang-x86_64-glm
```

6. Add the following directory to your Windows environment variable:
```text
MSYS2_ROOT : your-installed-msys2-path
Path : %MSYS2_ROOT%\clang64\bin
```

7. Restart the terminal or your PC after updating the environment variables.


#### for Linux (Ubuntu 24.04)


```bash
sudo apt install -y clang clang-tools make cmake libopencv-dev libvulkan-dev libglm-dev ninja-build libgtk-3-dev git wget xz-utils

wget https://gmplib.org/download/gmp/gmp-6.3.0.tar.xz
# You can get any version from https://gmplib.org/download/snapshot/gmp-next/
tar -xf gmp-6.3.0.tar.xz
cd gmp-6.3.0
mkdir build
cd build


../configure CFLAGS="-O3 -march=native"
make -j$(nproc)
sudo make install
cd ~

sudo ldconfig

```


### Build
The dependency [libgmp](https://github.com/sethtroisi/libgmp/tree/master) must be built separately.

```bash

git clone https://github.com/Merutilm/RFF-2.0
cd RFF-2.0
mkdir extern
cd extern
git clone https://github.com/ocornut/imgui
cd ..
mkdir build

cmake -B build -G "Ninja" -S . \
-DCMAKE_C_COMPILER=clang \
-DCMAKE_CXX_COMPILER=clang++ \
-DCMAKE_BUILD_TYPE=Release

cmake --build build -j$(nproc)

./build/bin/RFF
```



### Troubleshooting
#### for Windows
If the built application does not launch:

- Make sure `clang64\bin` is added to your `Path`
- Restart the terminal or your PC
- Launch the application from `clang64.exe`
- Verify required DLLs exist in `clang64\bin`

```bash
mkdir build

cmake -B build -G "Ninja" -S . \
-DCMAKE_C_COMPILER=clang \
-DCMAKE_CXX_COMPILER=clang++ \
-DCMAKE_BUILD_TYPE=Release \
-DSAFE_DEX_OPERATOR=ON

cmake --build build
```


## Features
- The status bar on the window means the following (from left to right):

1. The iterations of the pixel pointed to by the mouse cursor
2. The zoom of current location.
3. The estimated period of this location. (The number in parentheses is the length of the Reference and `MPA` array.)
4. The elapsed time since the calculation started
5. The Process

- Video renderer is built-in!
1. Use `Dynamic Map` or `Static Map` to generate video `keyframes`. \
This option is in `Data Settings` in `Video` menu.
2. `Dynamic Map` stores whole iteration data each pixel. It requires large capacity. \
the extension is `.rfm`.
3. `Static Map` stores as `image` and `info` files. It requires less capacity but also the most `shaders` are disabled. \
the extension of `info` file is `.rfsm`.
4. Export your own Video using existing `keyframes`.


- Find the nearest Minibrot with `Locate Minibrot` in `Explore` menu.

- More features will be added soon.

## Known Issues & Problems
- The program was compiled with -Ofast, so sometimes results incorrect image at some location.
- This is weak for complex spiral patterns and mandelbrot plane, because there are only formulas for the  recursive julia sets.  
  I will add that formulas in the future.
- The `Locate Minibrot` algorithm is currently inefficient. It is 50% slower than `kf2`.

- An issue occurs where the reference calculation slows down unusually at the certain very deep locations.

- No viable algorithms for interior detection(Coming soon). 
it will slow down the speed for interior pixels.

- I will do my best to accelerate as reference calculations account for more than 90% of the total
