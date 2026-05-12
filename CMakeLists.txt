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

set(CLANG64_DIRS "D:/merutilm/Developments/cpp/extern/msys/clang64")

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

target_link_directories(RFF PRIVATE
        ${CMAKE_SOURCE_DIR}/lib
)

target_link_libraries(RFF PRIVATE
        # GMP Linker
        gmp
        # Win32 Linker
        comctl32
        # OpenCV Linkers
        opencv_core
        opencv_imgcodecs
        opencv_imgproc
        opencv_videoio
        opencv_highgui
        # Vulkan Helper
        vulkan_helper
)

add_custom_target(spirv ALL COMMAND powershell -ExecutionPolicy Bypass -File "${CMAKE_CURRENT_SOURCE_DIR}/compile.ps1")
add_dependencies(RFF spirv)
