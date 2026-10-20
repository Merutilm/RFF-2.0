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
│  Debug               ██░░░░░░░░   Debug-friendly, optimized for debugging │
│  Release             ████████░░   Balanced performance and stability      │
│  Native              ██████████   Maximum performance for the host CPU    │
│  RelWithDebInfo      ███████░░░   Optimized build with debug symbols      │
│                                                                           │
└───────────────────────────────────────────────────────────────────────────┘

┌─ USAGE ───────────────────────────────────────────────────────────────────┐
│                                                                           │
│   cmake -G <generator> -B <build-dir> \\                                   │
│         -DCMAKE_TOOLCHAIN_FILE=<toolchain> \\                              │
│         -DRFF_BUILD_PROFILE=<RFF_PROFILE>                                 │
│                                                                           │
└───────────────────────────────────────────────────────────────────────────┘

    Example · Windows / Ninja / Native Performance

        cmake -G Ninja -B build \\
            -DCMAKE_TOOLCHAIN_FILE=windows-x86_64.cmake \\
            -DRFF_BUILD_PROFILE=Native

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

set(RFF_HELP_SUBMODULE_IMGUI "
┌───────────────────────────────────────────────────────────────────────────┐
│                                                                           │
│   The ImGui submodule has not been initialized.                           │
│   Please run the following command in the project root:                   │
│                                                                           │
│       git submodule update --init --recursive                             │
│                                                                           │
└───────────────────────────────────────────────────────────────────────────┘
"
)