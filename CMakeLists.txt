cmake_minimum_required(VERSION 3.30)
project(RFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_RPATH "$ORIGIN")

include(cmake/Constants.cmake)
include(cmake/Help.cmake)
include(cmake/Sources.cmake)
include(cmake/Macros.cmake)
include(cmake/Definition.cmake)
include(cmake/CompileOptions.cmake)
include(cmake/Dependencies.cmake)

message(${RFF_HELP_BUILD_RESULT})