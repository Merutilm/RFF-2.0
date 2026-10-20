
if (RFF_DEBUG)
    set(CMAKE_BUILD_TYPE Debug)
    target_compile_options(RFF PRIVATE
            -O0
    )
elseif (RFF_RELEASE)
    set(CMAKE_BUILD_TYPE Release)
    target_compile_options(RFF PRIVATE
            -O3 -ffast-math -funroll-loops -march=x86-64 -mtune=generic -mllvm -inline-threshold=${RFF_INLINE_THRESHOLD}
    )
elseif (RFF_NATIVE)
    set(CMAKE_BUILD_TYPE Release)
    target_compile_options(RFF PRIVATE
            -O3 -ffast-math -funroll-loops -march=native -mtune=native -mllvm -inline-threshold=${RFF_INLINE_THRESHOLD}
    )
elseif (RFF_RELWITHDEBINFO)
    set(CMAKE_BUILD_TYPE RelWithDebInfo)
    target_compile_options(RFF PRIVATE
            -O3 -ffast-math -fno-omit-frame-pointer -funroll-loops -march=native -mtune=native -mllvm -inline-threshold=${RFF_INLINE_THRESHOLD}
    )
else ()
    message("${RFF_HELP_BUILD}")
    message(FATAL_ERROR "Configuration aborted.")
endif ()

if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    message("${RFF_HELP_BUILD}")
    message(FATAL_ERROR "Configuration aborted.")
endif()

if(NOT EXISTS "${CMAKE_TOOLCHAIN_FILE}")
    message("${RFF_HELP_BUILD}")
    message(FATAL_ERROR "CMAKE_TOOLCHAIN_FILE not found!")
endif()