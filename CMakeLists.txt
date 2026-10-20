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


message(
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