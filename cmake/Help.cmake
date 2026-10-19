set(RFF_BUILD_HELP
        "
Compiler flag is not set.
(Hint: You can set the compiler flag)

==================== Toolchain list ==================
    linux-x86_64.cmake      |   For 64bit Linux.
    windows-x86_64.cmake    |   For 64bit Windows.
==================== Build type list =================
    RFF_DEBUG               |   Speed [=>        ]  Easy to debug
    RFF_RELEASE             |   Speed [======>   ]  General (The GitHub release is basically this build)
    RFF_NATIVE              |   Speed [=========>]  It works only on your CPU!
    RFF_RELWITHDEBINFO      |   Speed [======>   ]  For profiling
======================================================

usage: cmake -G <...> -B <...> -DCMAKE_TOOLCHAIN_FILE=<...> -D<...>=ON

example: cmake -G Ninja -B build -DCMAKE_TOOLCHAIN_FILE=windows-x86_64.cmake -DRFF_NATIVE=ON
"
)