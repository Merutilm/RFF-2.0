set(RFF_HELP_BUILD
        "
╔═══════════════════════════════════════════════════════════════════════════╗
║                         RFF · BUILD CONFIGURATION                         ║
╚═══════════════════════════════════════════════════════════════════════════╝

  No build configuration specified.
  Hint: Select a build profile by enabling one of the compiler flags below.

┌─ TOOLCHAINS ──────────────────────────────────────────────────────────────┐
│                                                                           │
│  windows-x86_64.cmake     64-bit Windows                                  │
│  linux-x86_64.cmake       64-bit Linux                                    │
│                                                                           │
└───────────────────────────────────────────────────────────────────────────┘

┌─ BUILD PROFILES ──────────────────────────────────────────────────────────┐
│  Profile             Performance  Description                             │
│                                                                           │
│  RFF_DEBUG           ██░░░░░░░░   Debug-friendly, optimized for debugging │
│  RFF_RELEASE         ████████░░   Balanced performance and stability      │
│  RFF_NATIVE          ██████████   Maximum performance for the host CPU    │
│  RFF_RELWITHDEBINFO  ███████░░░   Optimized build with debug symbols      │
│                                                                           │
└───────────────────────────────────────────────────────────────────────────┘

┌─ USAGE ───────────────────────────────────────────────────────────────────┐
│                                                                           │
│   cmake -G <generator> -B <build-dir> \\                                  │
│         -DCMAKE_TOOLCHAIN_FILE=<toolchain> \\                             │
│         -D<RFF_PROFILE>=ON                                                │
│                                                                           │
└───────────────────────────────────────────────────────────────────────────┘

    Example · Windows / Ninja / Native Performance

        cmake -G Ninja -B build \\
            -DCMAKE_TOOLCHAIN_FILE=windows-x86_64.cmake \\
            -DRFF_NATIVE=ON

─────────────────────────────────────────────────────────────────────────────
    RFF Build System
    Choose a toolchain and build profile to get started.
─────────────────────────────────────────────────────────────────────────────
"
)


set(RFF_HELP_MSYS2 "

option MSYS2_CLANG is not set. you must provide the root of clang64 path
if you are using Windows environment.

┌─ USAGE ───────────────────────────────────────────────────────────────────┐
│   cmake -G <...> -B <...> -DMSYS2_CLANG=<...>                             │
└───────────────────────────────────────────────────────────────────────────┘

    Example ·
        cmake -G Ninja -B build -DMSYS2_CLANG=\"C:/msys64/clang64\"
"
)


set(RFF_HELP_BUILD_RESULT
        "

╔══════════════════════════ BUILD ENVIRONMENT ══════════════════════════╗
║                                                                       ║
║    TARGET                                                             ║
║    Operating System  : ${CMAKE_SYSTEM_NAME}
║    Processor         : ${CMAKE_SYSTEM_PROCESSOR}
║    Build Type        : ${CMAKE_BUILD_TYPE}
║                                                                       ║
║    COMPILERS                                                          ║
║    C Compiler        : ${CMAKE_C_COMPILER}
║    C++ Compiler      : ${CMAKE_CXX_COMPILER}
║                                                                       ║
║    OUTPUT
║    Runtime Directory : ${RFF_OUTPUT_DIR}
║                                                                       ║
╚═══════════════════════════════════════════════════════════════════════╝

"
)