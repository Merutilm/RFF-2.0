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


get_filename_component(RFF_OUTPUT_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}" ABSOLUTE)
message(
        "
══════════════════════════════ BUILD PROFILE ════════════════════════════════

    TARGET
    Operating System  : ${CMAKE_SYSTEM_NAME}
    Processor         : ${CMAKE_SYSTEM_PROCESSOR}
    Build Type        : ${CMAKE_BUILD_TYPE}

    COMPILERS
    C Compiler        : ${CMAKE_C_COMPILER}
    C++ Compiler      : ${CMAKE_CXX_COMPILER}

    OUTPUT
    Runtime Directory : ${RFF_OUTPUT_DIR}

═════════════════════════════════════════════════════════════════════════════

"
)