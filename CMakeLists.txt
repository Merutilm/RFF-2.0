cmake_minimum_required(VERSION 3.30)
project(RFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_BUILD_RPATH "$ORIGIN")

include(cmake/Sources.cmake)
include(cmake/Constants.cmake)
include(cmake/Macros.cmake)
include(cmake/Definition.cmake)
include(cmake/Help.cmake)

if (WIN32)
    if (DEFINED ENV{MSYS2_ROOT})
        set(CLANG64_DIRS "$ENV{MSYS2_ROOT}/clang64")
        set(CLANG64_LIBS "${CLANG64_DIRS}/lib")
    else ()
        message(FATAL_ERROR "Environment Variable of MSYS2 is not set")
    endif ()
endif ()

include(cmake/CompileOptions.cmake)
include(cmake/Dependencies.cmake)


message(STATUS "========== Build Environment ==========")

message(STATUS "Target OS: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Target Processor: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER}")

message(STATUS "=======================================")