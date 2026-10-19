cmake_minimum_required(VERSION 3.30)
project(RFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_RPATH "$ORIGIN")

include(cmake/Sources.cmake)
include(cmake/Constants.cmake)
include(cmake/Macros.cmake)
include(cmake/Definition.cmake)
include(cmake/Help.cmake)
include(cmake/CompileOptions.cmake)
include(cmake/Dependencies.cmake)

message(STATUS "

========== Build Environment ==========

Target OS: ${CMAKE_SYSTEM_NAME}
Target Processor: ${CMAKE_SYSTEM_PROCESSOR}
Build Type: ${CMAKE_BUILD_TYPE}

C Compiler: ${CMAKE_C_COMPILER}
C++ Compiler: ${CMAKE_CXX_COMPILER}

Output Directory: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}

=======================================

")