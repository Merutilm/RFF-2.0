cmake_minimum_required(VERSION 3.30)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_RPATH "$ORIGIN")

message("
═════════════════════════════════ TOOLCHAIN ═════════════════════════════════
")

include(cmake/Constants.cmake)
include(cmake/Help.cmake)
include(cmake/Macros.cmake)
include(cmake/Toolchain.cmake)

message("
════════════════════════════ CONFIGURATION START ════════════════════════════
")

project(RFF)

include(cmake/Sources.cmake)
include(cmake/Definition.cmake)
include(cmake/BuildProfiles.cmake)
include(cmake/Dependencies.cmake)



# detect system processor

if(${RFF_BUILD_PROFILE} STREQUAL "Native")
    set(CLANG_PROBE "${CMAKE_CURRENT_BINARY_DIR}/cpu_probe.cpp")
    file(WRITE "${CLANG_PROBE}" "int main() { return 0; }\n")

    execute_process(
            COMMAND
            ${CMAKE_CXX_COMPILER} -march=native -\#\#\# -c -x c++ ${CLANG_PROBE}
            OUTPUT_VARIABLE CLANG_STDOUT
            ERROR_VARIABLE CLANG_STDERR
            RESULT_VARIABLE CLANG_RESULT
    )

    set(CLANG_OUTPUT "${CLANG_STDOUT}\n${CLANG_STDERR}")

    string(REGEX MATCH
            "\"-target-cpu\" \"([^\"]+)\""
            CLANG_CPU_MATCH
            "${CLANG_OUTPUT}"
    )

    set(RFF_CPU_TARGET ${CMAKE_MATCH_1})
    file(REMOVE ${CLANG_PROBE})
else()
    set(RFF_CPU_TARGET "x86_64-generic")
endif()

# output dir
get_filename_component(RFF_OUTPUT_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}" ABSOLUTE)
message("
══════════════════════════════ BUILD PROFILE ════════════════════════════════

    TARGET
    Operating System  : ${CMAKE_SYSTEM_NAME}
    Target Processor  : ${RFF_CPU_TARGET}
    Build Type        : ${CMAKE_BUILD_TYPE}

    COMPILERS
    C Compiler        : ${CMAKE_C_COMPILER}
    C++ Compiler      : ${CMAKE_CXX_COMPILER}

    OUTPUT
    Runtime Directory : ${RFF_OUTPUT_DIR}

═════════════════════════════════════════════════════════════════════════════

"
)