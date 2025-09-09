# RFF 2.0


## What is RFF?

**RFF** is an abbreviation for <u>**Ridiculously Fast Fractal**</u>.

- As the name suggests, its priority is ONLY Speed, and defaulted for the fastest settings.


## Overview
### Important : This program is **NOT COMPATIBLE** with **RFF(Java)** file extensions!

- A program that achieves extremely fast `Power-2 Mandelbrot set`.

- The application is built with the Vulkan API.

- This program uses `Fast-period-guessing`*(a.k.a. FPG)* which I developed. It automatically generates the `longest period` of the selected location.
This value is unmodifiable.

- This program uses an `Multilevel Periodic Approximation` algorithm which I developed.
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
