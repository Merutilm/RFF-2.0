

if (NOT DEFINED RFF_BUILD_PROFILE)
    message("${RFF_HELP_BUILD}")
    message(FATAL_ERROR "Toolchain file not specified!")
endif ()

set(VALID_PROFILES Debug Release Native RelWithDebInfo)
list(FIND VALID_PROFILES "${RFF_BUILD_PROFILE}" PROFILE_INDEX)

if(PROFILE_INDEX EQUAL -1)
    message("${RFF_HELP_BUILD}")
    message(FATAL_ERROR "Invalid build profile: ${RFF_BUILD_PROFILE}")
endif()

if (NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    message("${RFF_HELP_BUILD}")
    message(FATAL_ERROR "Toolchain file not specified!")
endif ()

if (NOT EXISTS "${CMAKE_SOURCE_DIR}/${CMAKE_TOOLCHAIN_FILE}")
    message("${RFF_HELP_BUILD}")
    message(FATAL_ERROR "CMAKE_TOOLCHAIN_FILE not found!")
endif ()


get_filename_component(RFF_OUTPUT_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}" ABSOLUTE)

message("
    The toolchain file is ${CMAKE_TOOLCHAIN_FILE}
    The c compiler is ${CMAKE_C_COMPILER}
    The c++ compiler is ${CMAKE_CXX_COMPILER}
    The build profile is ${RFF_BUILD_PROFILE}
    The build output is ${RFF_OUTPUT_DIR}
")