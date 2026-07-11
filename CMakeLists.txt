cmake_minimum_required(VERSION 3.30)
project(RFF)


if (NOT UNIX)
    if (NOT DEFINED ENV{MSYS2_ROOT})
        set(CLANG64_DIRS "$ENV{MSYS2_ROOT}/clang64")
        set(CLANG64_LIBS "${CLANG64_DIRS}/lib")
    endif ()
endif ()


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_executable(
        RFF
        src/rff2/ui/Main.cpp
        src/rff2/io/RFFDynamicMapBinary.cpp
        src/rff2/parallel/ParallelRenderState.cpp
        src/rff2/mrthy/MPAPeriod.cpp
        src/rff2/ui/SettingsWindow.cpp
        src/rff2/ui/CallbackFile.cpp
        src/rff2/ui/CallbackFractal.cpp
        src/rff2/ui/CallbackRender.cpp
        src/rff2/ui/CallbackShader.cpp
        src/rff2/locator/MB2Locator.cpp
        src/rff2/ui/CallbackExplore.cpp
        src/rff2/preset/shader/palette/ShdPalettePresets.cpp
        src/rff2/preset/shader/fog/ShdFogPresets.cpp
        src/rff2/preset/shader/color/ShdColorPresets.cpp
        src/rff2/preset/shader/slope/ShdSlopePresets.cpp
        src/rff2/preset/shader/stripe/ShdStripePresets.cpp
        src/rff2/preset/shader/bloom/ShdBloomPresets.cpp
        src/rff2/preset/resolution/ResolutionPresets.cpp
        src/rff2/preset/render/RenderPresets.cpp
        src/rff2/preset/calc/CalculationPresets.cpp
        src/rff2/ui/CallbackVideo.cpp
        src/rff2/ui/IOUtilities.cpp
        src/rff2/io/RFFStaticMapBinary.cpp
        src/rff2/io/RFFBinary.cpp
        src/rff2/io/RFFLocationBinary.cpp
        src/rff2/io/KFRColorLoader.cpp
        extern/stb_image.c
        src/rff2/vulkan/GPCIterationPalette.cpp
        src/rff2/ui/RFFApplication.cpp
        src/rff2/vulkan/GPCStripe.cpp
        src/rff2/vulkan/GPCSlope.cpp
        src/rff2/vulkan/GPCColor.cpp
        src/rff2/vulkan/GPCFog.cpp
        src/rff2/vulkan/GPCDownsampleForBlur.cpp
        src/rff2/vulkan/GPCBloomThreshold.cpp
        src/rff2/vulkan/CPCBoxBlur.cpp
        src/rff2/vulkan/GPCBloom.cpp
        src/rff2/vulkan/GPCLinearInterpolation.cpp
        src/rff2/vulkan/GPCPresent.cpp
        src/rff2/ui/VideoWindow.cpp
        src/rff2/ui/VideoWindowRenderManager.cpp
        src/rff2/vulkan/CPC2MapIterationStripe.cpp
        src/rff2/vulkan/GPCStaticImage2Map.cpp
        src/rff2/vulkan/CPCImageRGBA2BGR.cpp
        src/rff2/settings/FrtReferenceSettings.hpp
        src/rff2/settings/FrtPerturbSettings.hpp
        src/rff2/settings/FrtGeneralSettings.hpp
        src/rff2/calc/calculatable.hpp
        src/rff2/calc/complex.hpp
)

target_compile_options(RFF PRIVATE
        #Debug options
        $<$<CONFIG:Debug>:-O0>
        #Release options
        $<$<CONFIG:Release>:-O3 -ffast-math -fno-associative-math -funroll-loops -march=native -mllvm -inline-threshold=4000>
)

add_subdirectory(src/vulkan_helper)


target_link_libraries(RFF PRIVATE
        # Vulkan Helper
        vulkan_helper
)

target_include_directories(RFF PRIVATE
        # extern sources
        ${CMAKE_SOURCE_DIR}/extern
        # clang include dirs
        ${CMAKE_SOURCE_DIR}/include
        # clang include dirs
        ${CLANG64_DIRS}/include
        # opencv4 include dirs
)
if (WIN32)
    target_link_libraries(
            RFF PRIVATE
            # GMP Linker
            ${CLANG64_LIBS}/libgmp.a
            # OpenCV Linkers
            ${CLANG64_LIBS}/libopencv_core.dll.a
            ${CLANG64_LIBS}/libopencv_imgcodecs.dll.a
            ${CLANG64_LIBS}/libopencv_imgproc.dll.a
            ${CLANG64_LIBS}/libopencv_videoio.dll.a
            ${CLANG64_LIBS}/libopencv_highgui.dll.a
    )


    target_include_directories(RFF PRIVATE
            # opencv4 include dirs
            ${CLANG64_DIRS}/include/opencv4
    )
elseif (APPLE)
    target_link_libraries(RFF PRIVATE "-framework Cocoa")
    # TODO non compilable
elseif (UNIX)

    macro(find_static_library OUT_VAR LIB_NAME)
        find_library(${OUT_VAR}
                NAMES lib${LIB_NAME}.a
                PATHS
                /usr/local/lib
                /usr/lib
                /usr/lib/x86_64-linux-gnu
                REQUIRED
        )
    endmacro()

    find_package(OpenCV REQUIRED)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)


    find_static_library(GMP_STATIC gmp)

    target_link_directories(RFF PRIVATE ${GTK3_LIBRARY_DIRS})
    target_compile_options(RFF PRIVATE ${GTK3_CFLAGS_OTHER})

    target_link_libraries(RFF PRIVATE
            # GMP Linker
            ${GMP_STATIC}
            # gtk libraries
            ${GTK3_LIBRARIES}
            # OpenCV Linkers
            ${OpenCV_LIBS}
    )
    target_include_directories(RFF PRIVATE
            # opencv4 include dirs
            ${OpenCV_INCLUDE_DIRS}
            # gtk
            ${GTK3_INCLUDE_DIRS}
    )

endif ()

macro(RFF_DEFINITION def str flag)
    option(${def} ${str} ${flag})
    if (${def})
        message(STATUS "${def} is Enabled. ${str}")
        target_compile_definitions(RFF PUBLIC ${def})
    else ()
        message(STATUS "${def} is Disabled. Skipping")
    endif ()
endmacro()



include(FetchContent)

FetchContent_Declare(
        nfd
        GIT_REPOSITORY https://github.com/btzy/nativefiledialog-extended.git
        GIT_TAG master
)

FetchContent_MakeAvailable(nfd)

target_link_libraries(RFF PRIVATE nfd)

RFF_DEFINITION(SAFE_DEX_OPERATOR "Better Quality, Slower Speed for Deep Zoom" OFF)
