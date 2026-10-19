set(RFF_HELP_BUILD
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

set(RFF_HELP_MSYS2
        "option MSYS2_CLANG is not set. you must provide the root of clang64 path.
usage: cmake -G <...> -B <...> -DMSYS2_CLANG=<...>

example: cmake -G Ninja -B build -DMSYS2_CLANG=\"C:/msys64/clang64\"
"
)