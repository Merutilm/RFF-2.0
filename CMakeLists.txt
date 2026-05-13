cmake_minimum_required(VERSION 3.31)
project(RFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_executable(
        RFF
        res/resources.rc
        src/rff2/ui/Main.cpp
        src/rff2/io/RFFDynamicMapBinary.cpp
        src/rff2/formula/LightMB2Reference.cpp
        src/rff2/parallel/ParallelRenderState.cpp
        src/rff2/formula/LightMB2Perturbator.cpp
        src/rff2/mrthy/MPAPeriod.cpp
        src/rff2/ui/SettingsWindow.cpp
        src/rff2/ui/CallbackFile.cpp
        src/rff2/ui/CallbackFractal.cpp
        src/rff2/ui/CallbackRender.cpp
        src/rff2/ui/CallbackShader.cpp
        src/rff2/locator/MB2Locator.cpp
        src/rff2/ui/CallbackExplore.cpp
        src/rff2/formula/DeepMB2Reference.cpp
        src/rff2/formula/DeepMB2Perturbator.cpp
        src/rff2/mrthy/LightPAGenerator.cpp
        src/rff2/mrthy/DeepPAGenerator.cpp
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
        src/rff2/ui/AppRenderManager.cpp
        src/rff2/ui/AppLauncher.cpp
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
)


target_compile_options(RFF PRIVATE
        #Debug options
        $<$<CONFIG:Debug>:-O0>
        #Release options
        $<$<CONFIG:Release>:-O3 -ffast-math -funroll-loops -march=native>
)

target_compile_definitions(RFF
        PRIVATE
        VK_USE_PLATFORM_WIN32_KHR
)

add_subdirectory(src/vulkan_helper)

if(NOT DEFINED ENV{MSYS2_ROOT})
    message(FATAL_ERROR "MSYS2_ROOT not set. Add the your Windows environment variable named MSYS2_ROOT.")
endif()

set(CLANG64_DIRS "$ENV{MSYS2_ROOT}/clang64")
set(CLANG64_LIBS "${CLANG64_DIRS}/lib")

target_include_directories(RFF PRIVATE
        # extern sources
        ${CMAKE_SOURCE_DIR}/extern
        # clang include dirs
        ${CMAKE_SOURCE_DIR}/include
        # clang include dirs
        ${CLANG64_DIRS}/include
        # opencv4 include dirs
        ${CLANG64_DIRS}/include/opencv4
)

target_link_libraries(RFF PRIVATE
        # GMP Linker
        ${CLANG64_LIBS}/libgmp.a
        # Win32 Linker
        comctl32
        # OpenCV Linkers
        ${CLANG64_LIBS}/libopencv_core.dll.a
        ${CLANG64_LIBS}/libopencv_imgcodecs.dll.a
        ${CLANG64_LIBS}/libopencv_imgproc.dll.a
        ${CLANG64_LIBS}/libopencv_videoio.dll.a
        ${CLANG64_LIBS}/libopencv_highgui.dll.a
        # Vulkan Helper
        vulkan_helper
)
