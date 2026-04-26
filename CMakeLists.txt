cmake_minimum_required(VERSION 3.31)
project(RFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/mingw)

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
        src/rff2/ui/SettingsMenu.cpp
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
        src/vulkan_helper/impl/GraphicsContextWindow.cpp
        src/vulkan_helper/impl/ValidationLayer.cpp
        src/vulkan_helper/impl/Instance.cpp
        src/vulkan_helper/impl/Surface.cpp
        src/vulkan_helper/impl/PhysicalDeviceLoader.cpp
        src/vulkan_helper/impl/LogicalDevice.cpp
        src/vulkan_helper/impl/Swapchain.cpp
        src/vulkan_helper/impl/CommandPool.cpp
        src/vulkan_helper/impl/CommandBuffer.cpp
        src/vulkan_helper/impl/Descriptor.cpp
        src/vulkan_helper/impl/BufferObject.cpp
        src/vulkan_helper/impl/RenderPass.cpp
        src/vulkan_helper/impl/DescriptorSetLayout.cpp
        src/vulkan_helper/impl/PipelineLayout.cpp
        src/vulkan_helper/impl/ShaderModule.cpp
        src/vulkan_helper/impl/Core.cpp
        src/vulkan_helper/impl/Framebuffer.cpp
        src/vulkan_helper/impl/SyncObject.cpp
        src/vulkan_helper/executor/RenderPassFullscreenRecorder.cpp
        src/vulkan_helper/impl/VertexBuffer.cpp
        src/vulkan_helper/impl/Uniform.cpp
        src/vulkan_helper/impl/IndexBuffer.cpp
        src/vulkan_helper/impl/CombinedImageSampler.cpp
        src/vulkan_helper/configurator/GeneralGraphicsPipelineConfigurator.cpp
        src/vulkan_helper/impl/Engine.cpp
        src/vulkan_helper/executor/ScopedNewCommandBufferExecutor.cpp
        src/rff2/vulkan/GPCIterationPalette.cpp
        src/vulkan_helper/configurator/GeneralPostProcessGraphicsPipelineConfigurator.cpp
        src/rff2/ui/RenderScene.cpp
        src/rff2/ui/Application.cpp
        src/vulkan_helper/impl/Sampler.cpp
        src/vulkan_helper/impl/ShaderStorage.cpp
        src/rff2/vulkan/GPCStripe.cpp
        src/rff2/vulkan/GPCSlope.cpp
        src/rff2/vulkan/GPCColor.cpp
        src/rff2/vulkan/GPCFog.cpp
        src/rff2/vulkan/GPCDownsampleForBlur.cpp
        src/rff2/vulkan/GPCBloomThreshold.cpp
        src/vulkan_helper/impl/ComputeShaderPipeline.cpp
        src/vulkan_helper/impl/GraphicsPipeline.cpp
        src/vulkan_helper/executor/ScopedCommandBufferExecutor.cpp
        src/vulkan_helper/configurator/ComputePipelineConfigurator.cpp
        src/rff2/vulkan/CPCBoxBlur.cpp
        src/vulkan_helper/impl/Fence.cpp
        src/rff2/vulkan/GPCBloom.cpp
        src/rff2/vulkan/GPCLinearInterpolation.cpp
        src/vulkan_helper/impl/Semaphore.cpp
        src/rff2/vulkan/GPCPresent.cpp
        src/rff2/ui/VideoWindow.cpp
        src/rff2/ui/VideoRenderScene.cpp
        src/rff2/vulkan/CPC2MapIterationStripe.cpp
        src/vulkan_helper/context/WindowContext.cpp
        src/rff2/vulkan/GPCStaticImage2Map.cpp
        src/rff2/vulkan/CPCImageRGBA2BGR.cpp
)


target_compile_options(RFF PRIVATE
        #Debug options
        $<$<CONFIG:Debug>:-O0>
        #Release options
        $<$<CONFIG:Release>:-Ofast -funroll-loops -march=native>
)

target_compile_definitions(RFF
        PRIVATE
        VK_USE_PLATFORM_WIN32_KHR
)


find_package(Vulkan REQUIRED)

set(MINGW64_DIRS "D:/merutilm/Developments/cpp/extern/mingw/mingw64")
set(OPENCV_DIRS "D:/merutilm/Developments/cpp/extern/mingw/home/user/opencv/build")

target_include_directories(RFF PRIVATE
        # vulkan sdk directory
        ${Vulkan_INCLUDE_DIRS}
        # extern sources
        ${CMAKE_SOURCE_DIR}/extern
        # mingw include dirs
        ${MINGW64_DIRS}/include
        # opencv dir
        ${OPENCV_DIRS}/include
)

target_link_directories(RFF PRIVATE
        ${CMAKE_SOURCE_DIR}/lib/mingw
)


target_link_libraries(RFF PRIVATE
        # GMP Linker
        gmp
        # Win32 Linker
        comctl32
        # OpenCV Linkers
        opencv_core4140
        opencv_imgcodecs4140
        opencv_imgproc4140
        opencv_videoio4140
        opencv_highgui4140
        # Vulkan
        Vulkan::Vulkan
)

add_custom_target(spirv ALL COMMAND powershell -ExecutionPolicy Bypass -File "${CMAKE_CURRENT_SOURCE_DIR}/compile.ps1")
add_dependencies(RFF spirv)
