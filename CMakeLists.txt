cmake_minimum_required(VERSION 3.31)

project(RFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)



add_executable(
        RFF
        res/resources.rc
        src/rff2/ui/Main.cpp
        src/rff2/calc/fp_complex_mutable.h
        src/rff2/constants/Constants.hpp
        src/rff2/settings/Settings.h
        src/rff2/settings/FractalSettings.h
        src/rff2/settings/RenderSettings.h
        src/rff2/settings/ShaderSettings.h
        src/rff2/settings/ShdSlopeSettings.h
        src/rff2/settings/ShdStripeSettings.h
        src/rff2/settings/ShdColorSettings.h
        src/rff2/settings/ShdPaletteSettings.h
        src/rff2/settings/ShdFogSettings.h
        src/rff2/settings/VideoSettings.h
        src/rff2/settings/ShdBloomSettings.h
        src/rff2/settings/VidDataSettings.h
        src/rff2/settings/VidExportSettings.h
        src/rff2/settings/FrtMPASettings.h
        src/rff2/calc/fp_complex.h
        src/rff2/settings/FrtDecimalizeIterationMethod.h
        src/rff2/settings/FrtReferenceCompSettings.h
        src/rff2/settings/FrtReuseReferenceMethod.h
        src/rff2/settings/Selectable.h
        src/rff2/calc/fp_decimal_mutable.h
        src/rff2/calc/fp_decimal.h
        src/rff2/formula/Perturbator.h
        src/rff2/io/RFFDynamicMapBinary.cpp
        src/rff2/io/RFFDynamicMapBinary.h
        src/rff2/data/Matrix.h
        src/rff2/settings/ShdPalColorSmoothingMethod.h
        src/rff2/util/ColorUtils.h
        src/rff2/settings/ShdStripeType.h
        src/rff2/settings/VidAnimationSettings.h
        src/rff2/formula/LightMB2Reference.cpp
        src/rff2/formula/LightMB2Reference.h
        src/rff2/parallel/ParallelRenderState.cpp
        src/rff2/parallel/ParallelRenderState.h
        src/rff2/calc/rff_math.h
        src/rff2/mrthy/ArrayCompressionTool.h
        src/rff2/mrthy/ArrayCompressor.h
        src/rff2/parallel/ParallelArrayDispatcher.h
        src/rff2/ui/Utilities.h
        src/rff2/formula/LightMB2Perturbator.cpp
        src/rff2/formula/LightMB2Perturbator.h
        src/rff2/mrthy/LightPA.h
        src/rff2/mrthy/LightMPATable.h
        src/rff2/mrthy/MPAPeriod.cpp
        src/rff2/mrthy/MPAPeriod.h
        src/rff2/ui/SettingsWindow.cpp
        src/rff2/ui/SettingsWindow.hpp
        src/rff2/ui/SettingsMenu.cpp
        src/rff2/ui/SettingsMenu.hpp
        src/rff2/ui/CallbackFile.cpp
        src/rff2/ui/CallbackFile.hpp
        src/rff2/ui/CallbackFractal.cpp
        src/rff2/ui/CallbackFractal.hpp
        src/rff2/ui/CallbackRender.cpp
        src/rff2/ui/CallbackRender.hpp
        src/rff2/ui/CallbackShader.cpp
        src/rff2/ui/CallbackShader.hpp
        src/rff2/locator/MB2Locator.cpp
        src/rff2/locator/MB2Locator.h
        src/rff2/ui/CallbackExplore.cpp
        src/rff2/ui/CallbackExplore.hpp
        src/rff2/calc/dex.h
        src/rff2/formula/DeepMB2Reference.cpp
        src/rff2/formula/DeepMB2Reference.h
        src/rff2/formula/MB2Reference.h
        src/rff2/calc/dex_std.h
        src/rff2/calc/double_exp_math.h
        src/rff2/formula/DeepMB2Perturbator.cpp
        src/rff2/formula/DeepMB2Perturbator.h
        src/rff2/formula/MB2Perturbator.h
        src/rff2/mrthy/DeepPA.h
        src/rff2/mrthy/DeepMPATable.h
        src/rff2/mrthy/MPATable.h
        src/rff2/mrthy/PA.h
        src/rff2/ui/Callback.hpp
        src/rff2/mrthy/PAGenerator.h
        src/rff2/mrthy/LightPAGenerator.cpp
        src/rff2/mrthy/LightPAGenerator.h
        src/rff2/mrthy/DeepPAGenerator.cpp
        src/rff2/mrthy/DeepPAGenerator.h
        src/rff2/data/ApproxTableManager.h
        src/rff2/preset/shader/palette/ShdPalettePresets.h
        src/rff2/preset/shader/color/ShdColorPresets.h
        src/rff2/preset/shader/bloom/ShdBloomPresets.h
        src/rff2/preset/shader/fog/ShdFogPresets.h
        src/rff2/preset/shader/slope/ShdSlopePresets.h
        src/rff2/preset/shader/stripe/ShdStripePresets.h
        src/rff2/preset/shader/palette/ShdPalettePresets.cpp
        src/rff2/preset/shader/fog/ShdFogPresets.cpp
        src/rff2/preset/shader/color/ShdColorPresets.cpp
        src/rff2/preset/shader/slope/ShdSlopePresets.cpp
        src/rff2/preset/shader/stripe/ShdStripePresets.cpp
        src/rff2/preset/shader/bloom/ShdBloomPresets.cpp
        src/rff2/preset/Presets.h
        src/rff2/preset/resolution/ResolutionPresets.cpp
        src/rff2/preset/resolution/ResolutionPresets.h
        src/rff2/preset/render/RenderPresets.cpp
        src/rff2/preset/render/RenderPresets.h
        src/rff2/preset/calc/CalculationPresets.cpp
        src/rff2/preset/calc/CalculationPresets.h
        src/rff2/ui/CallbackVideo.cpp
        src/rff2/ui/CallbackVideo.hpp
        src/rff2/ui/IOUtilities.cpp
        src/rff2/ui/IOUtilities.h
        src/rff2/parallel/BackgroundThreads.h
        src/rff2/parallel/BackgroundThread.h
        src/rff2/io/RFFStaticMapBinary.cpp
        src/rff2/io/RFFStaticMapBinary.h
        src/rff2/io/RFFBinary.cpp
        src/rff2/io/RFFBinary.h
        src/rff2/io/RFFLocationBinary.cpp
        src/rff2/io/RFFLocationBinary.h
        src/rff2/io/KFRColorLoader.cpp
        src/rff2/io/KFRColorLoader.hpp
        extern/stb_image.c
        extern/stb_image.h
        src/vulkan_helper/handle/Handler.hpp
        src/vulkan_helper/impl/GraphicsContextWindow.cpp
        src/vulkan_helper/impl/GraphicsContextWindow.hpp
        src/vulkan_helper/impl/ValidationLayer.cpp
        src/vulkan_helper/impl/ValidationLayer.hpp
        src/vulkan_helper/core/exception.hpp
        src/vulkan_helper/impl/Instance.cpp
        src/vulkan_helper/impl/Instance.hpp
        src/vulkan_helper/impl/Surface.cpp
        src/vulkan_helper/impl/Surface.hpp
        src/vulkan_helper/impl/PhysicalDeviceLoader.cpp
        src/vulkan_helper/impl/PhysicalDeviceLoader.hpp
        src/vulkan_helper/struct/QueueFamilyIndices.hpp
        src/vulkan_helper/impl/LogicalDevice.cpp
        src/vulkan_helper/impl/LogicalDevice.hpp
        src/vulkan_helper/struct/StringHasher.hpp
        src/vulkan_helper/impl/Swapchain.cpp
        src/vulkan_helper/impl/Swapchain.hpp
        src/vulkan_helper/impl/CommandPool.cpp
        src/vulkan_helper/impl/CommandPool.hpp
        src/vulkan_helper/impl/CommandBuffer.cpp
        src/vulkan_helper/impl/CommandBuffer.hpp
        src/vulkan_helper/impl/Descriptor.cpp
        src/vulkan_helper/impl/Descriptor.hpp
        src/vulkan_helper/manage/HostDataObjectManager.hpp
        src/vulkan_helper/impl/BufferObject.cpp
        src/vulkan_helper/impl/BufferObject.hpp
        src/vulkan_helper/util/BufferImageUtils.hpp
        src/vulkan_helper/manage/DescriptorManager.hpp
        src/vulkan_helper/impl/Pipeline.hpp
        src/vulkan_helper/impl/RenderPass.cpp
        src/vulkan_helper/impl/RenderPass.hpp
        src/vulkan_helper/manage/PipelineManager.hpp
        src/vulkan_helper/impl/DescriptorSetLayout.hpp
        src/vulkan_helper/struct/DescriptorSetLayoutBuildType.hpp
        src/vulkan_helper/impl/PipelineLayout.hpp
        src/vulkan_helper/impl/DescriptorSetLayout.cpp
        src/vulkan_helper/impl/PipelineLayout.cpp
        src/vulkan_helper/impl/PushConstant.hpp
        src/vulkan_helper/impl/ShaderModule.hpp
        src/vulkan_helper/impl/ShaderModule.cpp
        src/vulkan_helper/manage/RenderPassManager.hpp
        src/vulkan_helper/struct/RenderPassAttachmentType.hpp
        src/vulkan_helper/impl/Core.cpp
        src/vulkan_helper/impl/Core.hpp
        src/vulkan_helper/handle/CoreHandler.hpp
        src/vulkan_helper/impl/Framebuffer.cpp
        src/vulkan_helper/impl/Framebuffer.hpp
        src/vulkan_helper/struct/RenderPassAttachment.hpp
        src/vulkan_helper/struct/ImageInitInfo.hpp
        src/vulkan_helper/impl/SyncObject.cpp
        src/vulkan_helper/impl/SyncObject.hpp
        src/vulkan_helper/executor/RenderPassFullscreenRecorder.cpp
        src/vulkan_helper/executor/RenderPassFullscreenRecorder.hpp
        src/vulkan_helper/struct/Vertex.hpp
        src/vulkan_helper/impl/VertexBuffer.cpp
        src/vulkan_helper/impl/VertexBuffer.hpp
        src/vulkan_helper/impl/HostDataObject.hpp
        src/vulkan_helper/impl/Uniform.cpp
        src/vulkan_helper/impl/Uniform.hpp
        src/vulkan_helper/repo/GlobalShaderModuleRepo.hpp
        src/vulkan_helper/context/WindowContext.hpp
        src/vulkan_helper/context/RenderContext.hpp
        src/vulkan_helper/hash/VectorHasher.hpp
        src/vulkan_helper/hash/PointerDerefEquals.hpp
        src/vulkan_helper/repo/Repository.hpp
        src/vulkan_helper/repo/Repositories.hpp
        src/vulkan_helper/repo/RepoAbstract.hpp
        src/vulkan_helper/manage/PipelineLayoutManager.hpp
        src/vulkan_helper/hash/BoostHasher.hpp
        src/vulkan_helper/repo/GlobalPipelineLayoutRepo.hpp
        src/vulkan_helper/impl/IndexBuffer.cpp
        src/vulkan_helper/impl/IndexBuffer.hpp
        src/vulkan_helper/impl/CombinedImageSampler.cpp
        src/vulkan_helper/impl/CombinedImageSampler.hpp
        src/vulkan_helper/configurator/GeneralGraphicsPipelineConfigurator.cpp
        src/vulkan_helper/configurator/GeneralGraphicsPipelineConfigurator.hpp
        src/vulkan_helper/repo/WindowLocalDescriptorRepo.hpp
        src/vulkan_helper/struct/DescriptorTemplateInfo.hpp
        src/rff2/vulkan/SharedDescriptorTemplate.hpp
        src/vulkan_helper/impl/Engine.cpp
        src/vulkan_helper/impl/Engine.hpp
        src/vulkan_helper/executor/ScopedNewCommandBufferExecutor.cpp
        src/vulkan_helper/executor/ScopedNewCommandBufferExecutor.hpp
        src/vulkan_helper/struct/PushConstantReserve.hpp
        src/vulkan_helper/configurator/RenderContextConfigurator.hpp
        src/rff2/vulkan/RCC1.hpp
        src/vulkan_helper/hash/DescriptorSetLayoutBuildTypeHasher.hpp
        src/vulkan_helper/context/ImageContext.hpp
        src/vulkan_helper/util/SwapchainUtils.hpp
        src/rff2/vulkan/GPCIterationPalette.cpp
        src/rff2/vulkan/GPCIterationPalette.hpp
        src/vulkan_helper/context/DescriptorUpdateContext.hpp
        src/vulkan_helper/core/safe_array.hpp
        src/vulkan_helper/util/BufferImageContextUtils.hpp
        src/vulkan_helper/configurator/GeneralPostProcessGraphicsPipelineConfigurator.cpp
        src/vulkan_helper/configurator/GeneralPostProcessGraphicsPipelineConfigurator.hpp
        src/rff2/ui/RenderScene.cpp
        src/rff2/ui/RenderScene.hpp
        src/rff2/ui/Application.cpp
        src/rff2/ui/Application.hpp
        src/rff2/constants/Win32Constants.hpp
        src/rff2/constants/FractalConstants.hpp
        src/rff2/constants/NumConstants.hpp
        src/rff2/constants/VideoConstants.hpp
        src/rff2/constants/ExtensionConstants.hpp
        src/rff2/constants/StatusConstants.hpp
        src/vulkan_helper/configurator/GraphicsPipelineConfigurator.hpp
        src/rff2/constants/VulkanWindowConstants.hpp
        src/vulkan_helper/struct/DescriptorTemplate.hpp
        src/vulkan_helper/impl/Sampler.cpp
        src/vulkan_helper/impl/Sampler.hpp
        src/vulkan_helper/repo/GlobalSamplerRepo.hpp
        src/vulkan_helper/hash/SamplerCreateInfoHasher.hpp
        src/vulkan_helper/hash/SamplerCreateInfoEquals.hpp
        src/vulkan_helper/impl/ShaderStorage.cpp
        src/vulkan_helper/impl/ShaderStorage.hpp
        src/vulkan_helper/struct/BufferLock.hpp
        src/vulkan_helper/core/logger.hpp
        src/vulkan_helper/util/DescriptorUpdater.hpp
        src/rff2/vulkan/GPCStripe.cpp
        src/rff2/vulkan/GPCStripe.hpp
        src/rff2/vulkan/GPCSlope.cpp
        src/rff2/vulkan/GPCSlope.hpp
        src/rff2/vulkan/GPCColor.cpp
        src/rff2/vulkan/GPCColor.hpp
        src/rff2/vulkan/GPCFog.cpp
        src/rff2/vulkan/GPCFog.hpp
        src/rff2/vulkan/GPCDownsampleForBlur.cpp
        src/rff2/vulkan/GPCDownsampleForBlur.hpp
        src/rff2/vulkan/GPCBloomThreshold.cpp
        src/rff2/vulkan/GPCBloomThreshold.hpp
        src/vulkan_helper/core/Factory.hpp
        src/vulkan_helper/util/Debugger.hpp
        src/vulkan_helper/util/PhysicalDeviceUtils.hpp
        src/vulkan_helper/util/GraphicsContextWindowProc.hpp
        src/vulkan_helper/impl/ComputeShaderPipeline.cpp
        src/vulkan_helper/impl/ComputeShaderPipeline.hpp
        src/vulkan_helper/impl/GraphicsPipeline.cpp
        src/vulkan_helper/impl/GraphicsPipeline.hpp
        src/vulkan_helper/configurator/PipelineConfigurator.hpp
        src/vulkan_helper/executor/ScopedCommandBufferExecutor.cpp
        src/vulkan_helper/executor/ScopedCommandBufferExecutor.hpp
        src/vulkan_helper/configurator/ComputePipelineConfigurator.cpp
        src/vulkan_helper/configurator/ComputePipelineConfigurator.hpp
        src/vulkan_helper/struct/InputAttachment.hpp
        src/rff2/vulkan/CPCBoxBlur.cpp
        src/rff2/vulkan/CPCBoxBlur.hpp
        src/rff2/vulkan/RCCPresent.hpp
        src/vulkan_helper/core/Config.hpp
        src/vulkan_helper/impl/Fence.cpp
        src/vulkan_helper/impl/Fence.hpp
        src/rff2/vulkan/RCCDownsampleForBlur.hpp
        src/rff2/vulkan/RCC3.hpp
        src/rff2/vulkan/GPCBloom.cpp
        src/rff2/vulkan/GPCBloom.hpp
        src/rff2/vulkan/RCC4.hpp
        src/rff2/vulkan/GPCLinearInterpolation.cpp
        src/rff2/vulkan/GPCLinearInterpolation.hpp
        src/rff2/vulkan/RCC5.hpp
        src/vulkan_helper/core/vkh_base.hpp
        src/vulkan_helper/core/vkh.hpp
        src/vulkan_helper/core/vkh_core.hpp
        src/vulkan_helper/hash/PipelineLayoutManagerEquals.hpp
        src/vulkan_helper/hash/VariantHasher.hpp
        src/vulkan_helper/impl/Semaphore.cpp
        src/vulkan_helper/impl/Semaphore.hpp
        src/vulkan_helper/context/SharedImageContext.hpp
        src/rff2/vulkan/SharedImageContextIndices.hpp
        src/vulkan_helper/util/BarrierUtils.hpp
        src/vulkan_helper/context/BufferContext.hpp
        src/vulkan_helper/struct/BufferInitInfo.hpp
        src/rff2/ui/RenderSceneRequests.hpp
        src/rff2/ui/RenderSceneRenderer.hpp
        src/rff2/vulkan/GPCPresent.cpp
        src/rff2/vulkan/GPCPresent.hpp
        src/rff2/ui/VideoWindow.cpp
        src/rff2/ui/VideoWindow.hpp
        src/rff2/ui/VideoRenderScene.cpp
        src/rff2/ui/VideoRenderScene.hpp
        src/rff2/ui/VideoRenderSceneRenderer.hpp
        src/rff2/vulkan/CPC2MapIterationStripe.cpp
        src/rff2/vulkan/CPC2MapIterationStripe.hpp
        src/rff2/vulkan/RCC1Vid.hpp
        src/rff2/vulkan/RCC2Vid.hpp
        src/rff2/vulkan/RCC3Vid.hpp
        src/rff2/vulkan/RCC4Vid.hpp
        src/rff2/vulkan/RCCDownsampleForBlurVid.hpp
        src/vulkan_helper/context/WindowContext.cpp
        src/vulkan_helper/impl/Renderer.hpp
        src/rff2/vulkan/RCC0.hpp
        src/vulkan_helper/core/allocator.hpp
        src/rff2/vulkan/GPCStaticImage2Map.cpp
        src/rff2/vulkan/GPCStaticImage2Map.hpp
        src/rff2/vulkan/CPCImageRGBA2BGR.cpp
        src/rff2/vulkan/CPCImageRGBA2BGR.hpp
        src/rff2/ui/VideoBufferCache.hpp
        src/rff2/util/profiler.hpp
        src/rff2/formula/Reference.hpp
        src/rff2/settings/FrtReferenceSyncSettings.hpp
        src/rff2/mem/single_alloc_permitted_memory_resource.hpp
        src/rff2/ui/VideoCodec.hpp
        src/rff2/ui/FFmpegContext.hpp
)
target_compile_options(RFF PRIVATE
        #Debug options
        $<$<CONFIG:Debug>:/Od /utf-8>
        #Release options
        $<$<CONFIG:Release>:/O2 /utf-8 /GL /fp:fast /arch:AVX2 /Qpar /Gw /GS->
)

target_compile_definitions(RFF
        PRIVATE
        VK_USE_PLATFORM_WIN32_KHR
)


find_package(Vulkan REQUIRED)
find_package(CUDAToolkit REQUIRED)

set(MSVC_INCLUDE_DIRS "D:/merutilm/Developments/cpp/extern/vcpkg/installed/x64-windows/include")
set(MSVC_LINK_DIRS
        $<$<CONFIG:Debug>:D:/merutilm/Developments/cpp/extern/vcpkg/installed/x64-windows/debug/lib>
        $<$<CONFIG:Release>:D:/merutilm/Developments/cpp/extern/vcpkg/installed/x64-windows/lib>
)


target_include_directories(RFF PRIVATE
        # standard include
        # {CMAKE_SOURCE_DIR}/include/mingw
        # extern sources
        ${CMAKE_SOURCE_DIR}/extern
        # vulkan sdk directory
        ${Vulkan_INCLUDE_DIRS}
        ${MSVC_INCLUDE_DIRS}
        ${CUDAToolkit_INCLUDE_DIRS}
)

target_link_directories(RFF PRIVATE
        ${MSVC_LINK_DIRS}
)

target_link_libraries(RFF PRIVATE
        # GMP
        gmp
        # FFmpeg
        avcodec
        avdevice
        avfilter
        avutil
        avformat
        swresample
        swscale
        # OpenCV
        $<$<CONFIG:Debug>:opencv_core4d>
        $<$<CONFIG:Debug>:opencv_imgcodecs4d>
        $<$<CONFIG:Debug>:opencv_imgproc4d>
        $<$<CONFIG:Debug>:opencv_videoio4d>
        $<$<CONFIG:Debug>:opencv_highgui4d>
        $<$<CONFIG:Release>:opencv_core4>
        $<$<CONFIG:Release>:opencv_imgcodecs4>
        $<$<CONFIG:Release>:opencv_imgproc4>
        $<$<CONFIG:Release>:opencv_videoio4>
        $<$<CONFIG:Release>:opencv_highgui4>
        # Win32 Linker
        comctl32
        # CUDA
        CUDA::nppicc
        CUDA::nppc
        CUDA::nppig
        CUDA::nppif
        CUDA::nppist
        CUDA::nppisu
        CUDA::cudart
        CUDA::cuda_driver
        # Vulkan
        Vulkan::Vulkan
)


add_custom_target(spirv ALL COMMAND powershell -ExecutionPolicy Bypass -File "${CMAKE_CURRENT_SOURCE_DIR}/compile.ps1")
add_dependencies(RFF spirv)
