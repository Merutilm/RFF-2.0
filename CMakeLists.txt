cmake_minimum_required(VERSION 3.31)
project(RFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_SOURCE_DIR}/bin)

add_compile_options(
        -O0
#        -Ofast
#        -march=native
#        -funroll-loops
)



add_executable(
        RFF
        res/resources.rc
        extern/glad.c
        extern/glad_wgl.c
        src/ui/Main.cpp
        src/opengl/GLShader.cpp
        src/opengl/GLRenderer.cpp
        src/opengl/GLRenderer.h
        src/calc/fp_complex_calculator.cpp
        src/calc/fp_complex_calculator.h
        src/ui/GLMainWindow.cpp
        src/ui/GLMainWindow.h
        src/constants/Constants.hpp
        src/settings/Settings.h
        src/settings/CalculationSettings.h
        src/settings/RenderSettings.h
        src/settings/ShaderSettings.h
        src/settings/SlopeSettings.h
        src/settings/StripeSettings.h
        src/settings/ColorSettings.h
        src/settings/PaletteSettings.h
        src/settings/FogSettings.h
        src/settings/VideoSettings.h
        src/settings/BloomSettings.h
        src/settings/DataSettings.h
        src/settings/ExportSettings.h
        src/settings/MpaSettings.h
        src/calc/fp_complex.cpp
        src/calc/fp_complex.h
        src/settings/DecimalizeIterationMethod.h
        src/settings/ReferenceCompressionSettings.h
        src/settings/ReuseReferenceMethod.h
        src/settings/Selectable.h
        src/calc/fp_decimal_calculator.cpp
        src/calc/fp_decimal_calculator.h
        src/calc/fp_decimal.cpp
        src/calc/fp_decimal.h
        src/ui/WGLContextLoader.cpp
        src/ui/WGLContextLoader.h
        src/ui/GLRenderScene.cpp
        src/ui/GLRenderScene.h
        src/formula/Perturbator.h
        src/ui/WGLScene.h
        src/ui/WGLScene.cpp
        src/io/RFFDynamicMapBinary.cpp
        src/io/RFFDynamicMapBinary.h
        src/data/Matrix.h
        src/opengl/GLRendererIteration.cpp
        src/opengl/GLRendererIteration.h
        src/settings/ColorSmoothingSettings.h
        src/data/ColorFloat.h
        src/settings/StripeType.h
        src/settings/AnimationSettings.h
        src/opengl/GLTimeRenderer.h
        src/opengl/GLTimeProvider.h
        src/formula/LightMandelbrotReference.cpp
        src/formula/LightMandelbrotReference.h
        src/parallel/ParallelRenderState.cpp
        src/parallel/ParallelRenderState.h
        src/calc/rff_math.h
        src/mrthy/ArrayCompressionTool.h
        src/mrthy/ArrayCompressor.h
        src/parallel/ParallelArrayDispatcher.h
        src/ui/Utilities.h
        src/formula/LightMandelbrotPerturbator.cpp
        src/formula/LightMandelbrotPerturbator.h
        src/mrthy/LightPA.h
        src/mrthy/LightMPATable.h
        src/mrthy/MPAPeriod.cpp
        src/mrthy/MPAPeriod.h
        src/ui/SettingsWindow.cpp
        src/ui/SettingsWindow.h
        src/ui/SettingsMenu.cpp
        src/ui/SettingsMenu.h
        src/ui/CallbackFile.cpp
        src/ui/CallbackFile.h
        src/ui/CallbackFractal.cpp
        src/ui/CallbackFractal.h
        src/ui/CallbackRender.cpp
        src/ui/CallbackRender.h
        src/opengl/GLMultipassRenderer.cpp
        src/opengl/GLMultipassRenderer.h
        src/opengl/GLRendererStripe.cpp
        src/opengl/GLRendererStripe.h
        src/opengl/GLRendererSlope.cpp
        src/opengl/GLRendererSlope.h
        src/opengl/GLRendererColor.cpp
        src/opengl/GLRendererColor.h
        src/opengl/GLRendererBoxBlur.cpp
        src/opengl/GLRendererBoxBlur.h
        src/opengl/GLRendererGaussianBlur.cpp
        src/opengl/GLRendererGaussianBlur.h
        src/opengl/GLRendererBloom.cpp
        src/opengl/GLRendererBloom.h
        src/opengl/GLRendererFog.cpp
        src/opengl/GLRendererFog.h
        src/opengl/GLRendererAntialiasing.cpp
        src/opengl/GLRendererAntialiasing.h
        src/ui/CallbackShader.cpp
        src/ui/CallbackShader.h
        src/locator/MandelbrotLocator.cpp
        src/locator/MandelbrotLocator.h
        src/ui/CallbackExplore.cpp
        src/ui/CallbackExplore.h
        src/calc/dex.h
        src/formula/DeepMandelbrotReference.cpp
        src/formula/DeepMandelbrotReference.h
        src/formula/MandelbrotReference.h
        src/calc/dex_std.h
        src/calc/double_exp_math.h
        src/formula/DeepMandelbrotPerturbator.cpp
        src/formula/DeepMandelbrotPerturbator.h
        src/formula/MandelbrotPerturbator.h
        src/mrthy/DeepPA.h
        src/mrthy/DeepMPATable.h
        src/mrthy/MPATable.h
        src/mrthy/PA.h
        src/ui/Callback.h
        src/mrthy/PAGenerator.h
        src/mrthy/LightPAGenerator.cpp
        src/mrthy/LightPAGenerator.h
        src/mrthy/DeepPAGenerator.cpp
        src/mrthy/DeepPAGenerator.h
        src/data/ApproxTableCache.h
        src/preset/shader/palette/PalettePresets.h
        src/preset/shader/color/ColorPresets.h
        src/preset/shader/bloom/BloomPresets.h
        src/preset/shader/fog/FogPresets.h
        src/preset/shader/slope/SlopePresets.h
        src/preset/shader/stripe/StripePresets.h
        src/preset/shader/palette/PalettePresets.cpp
        src/preset/shader/fog/FogPresets.cpp
        src/preset/shader/color/ColorPresets.cpp
        src/preset/shader/slope/SlopePresets.cpp
        src/preset/shader/stripe/StripePresets.cpp
        src/preset/shader/bloom/BloomPresets.cpp
        src/preset/Presets.h
        src/opengl/GLRendererDisplayer.cpp
        src/opengl/GLRendererDisplayer.h
        src/preset/resolution/ResolutionPresets.cpp
        src/preset/resolution/ResolutionPresets.h
        src/preset/render/RenderPresets.cpp
        src/preset/render/RenderPresets.h
        src/preset/calc/CalculationPresets.cpp
        src/preset/calc/CalculationPresets.h
        src/ui/CallbackVideo.cpp
        src/ui/CallbackVideo.h
        src/ui/IOUtilities.cpp
        src/ui/IOUtilities.h
        src/parallel/BackgroundThreads.h
        src/parallel/BackgroundThread.h
        src/ui/VideoWindow.cpp
        src/ui/VideoWindow.h
        src/ui/VideoRenderScene.cpp
        src/ui/VideoRenderScene.h
        src/opengl/GLRendererIteration2Map.cpp
        src/opengl/GLRendererIteration2Map.h
        src/opengl/GLShaderLoader.h
        src/opengl/GLShaderLoader.cpp
        src/opengl/GLShaderConfig.h
        src/opengl/GLShaderConfig.cpp
        src/io/RFFStaticMapBinary.cpp
        src/io/RFFStaticMapBinary.h
        src/io/RFFBinary.cpp
        src/io/RFFBinary.h
        src/opengl/GLRendererStatic2Image.cpp
        src/opengl/GLRendererStatic2Image.h
        src/io/RFFLocationBinary.cpp
        src/io/RFFLocationBinary.h
        src/opengl/GLRendererShdIteration2Map.h
        src/opengl/GLRendererColIteration2Map.cpp
        src/io/KFRColorLoader.cpp
        src/io/KFRColorLoader.hpp

        extern/stb_image.c
        extern/stb_image.h
        src/vulkan/handle/Handler.hpp
        src/vulkan/def/GraphicsContextWindow.cpp
        src/vulkan/def/GraphicsContextWindow.hpp
        src/vulkan/def/ValidationLayer.cpp
        src/vulkan/def/ValidationLayer.hpp
        src/vulkan/exception/exception.hpp
        src/vulkan/def/Instance.cpp
        src/vulkan/def/Instance.hpp
        src/vulkan/def/Surface.cpp
        src/vulkan/def/Surface.hpp
        src/vulkan/def/PhysicalDevice.cpp
        src/vulkan/def/PhysicalDevice.hpp
        src/vulkan/struct/QueueFamilyIndices.hpp
        src/vulkan/def/LogicalDevice.cpp
        src/vulkan/def/LogicalDevice.hpp
        src/vulkan/struct/StringHasher.hpp
        src/vulkan/def/Swapchain.cpp
        src/vulkan/def/Swapchain.hpp
        src/vulkan/def/CommandPool.cpp
        src/vulkan/def/CommandPool.hpp
        src/vulkan/def/CommandBuffer.cpp
        src/vulkan/def/CommandBuffer.hpp
        src/vulkan/def/Descriptor.cpp
        src/vulkan/def/Descriptor.hpp
        src/vulkan/manage/ShaderObjectManager.hpp
        src/vulkan/def/BufferObject.cpp
        src/vulkan/def/BufferObject.hpp
        src/vulkan/util/BufferImageUtils.hpp
        src/vulkan/manage/DescriptorManager.hpp
        src/vulkan/def/Pipeline.cpp
        src/vulkan/def/Pipeline.hpp
        src/vulkan/def/RenderPass.cpp
        src/vulkan/def/RenderPass.hpp
        src/vulkan/manage/PipelineManager.hpp
        src/vulkan/def/DescriptorSetLayout.hpp
        src/vulkan/struct/DescriptorSetLayoutBuildType.hpp
        src/vulkan/def/PipelineLayout.hpp
        src/vulkan/def/DescriptorSetLayout.cpp
        src/vulkan/def/PipelineLayout.cpp
        src/vulkan/manage/PushConstantManager.hpp
        src/vulkan/def/ShaderModule.hpp
        src/vulkan/def/ShaderModule.cpp
        src/vulkan/manage/RenderPassManager.hpp
        src/vulkan/struct/RenderPassAttachmentType.hpp
        src/vulkan/def/Core.cpp
        src/vulkan/def/Core.hpp
        src/vulkan/handle/CoreHandler.hpp
        src/vulkan/struct/WindowCreateInfo.hpp
        src/vulkan/def/Framebuffer.cpp
        src/vulkan/def/Framebuffer.hpp
        src/vulkan/struct/RenderPassAttachment.hpp
        src/vulkan/struct/ImageInitInfo.hpp
        src/vulkan/def/SyncObject.cpp
        src/vulkan/def/SyncObject.hpp
        src/vulkan/executor/RenderPassExecutor.cpp
        src/vulkan/executor/RenderPassExecutor.hpp
        src/vulkan/struct/Vertex.hpp
        src/vulkan/def/VertexBuffer.cpp
        src/vulkan/def/VertexBuffer.hpp
        src/vulkan/manage/CompleteShaderObjectManager.hpp
        src/vulkan/def/Uniform.cpp
        src/vulkan/def/Uniform.hpp
        src/vulkan/executor/Executor.hpp
        src/vulkan/repo/ShaderModuleRepo.hpp
        src/vulkan/context/WindowContext.hpp
        src/vulkan/context/RenderContext.hpp
        src/vulkan/hash/VectorHasher.hpp
        src/vulkan/hash/PointerDerefEquals.hpp
        src/vulkan/repo/Repo.hpp
        src/vulkan/repo/Repositories.hpp
        src/vulkan/repo/IRepo.hpp
        src/vulkan/manage/PipelineLayoutManager.hpp
        src/vulkan/hash/BoostHasher.hpp
        src/vulkan/repo/PipelineLayoutRepo.hpp
        src/vulkan/def/IndexBuffer.cpp
        src/vulkan/def/IndexBuffer.hpp
        src/vulkan/def/Sampler2D.cpp
        src/vulkan/def/Sampler2D.hpp
        src/vulkan/configurator/GeneralPipelineConfigurator.cpp
        src/vulkan/configurator/GeneralPipelineConfigurator.hpp
        src/vulkan/repo/SharedDescriptorRepo.hpp
        src/vulkan/struct/DescriptorName.hpp
        src/vulkan/template/DSSharedTemplate.hpp
        src/vulkan/def/Engine.cpp
        src/vulkan/def/Engine.hpp
        src/vulkan/configurator/Template2PipelineConfigurator.cpp
        src/vulkan/configurator/Template2PipelineConfigurator.hpp
        src/vulkan/executor/ScopedCommandExecutor.cpp
        src/vulkan/executor/ScopedCommandExecutor.hpp
        src/vulkan/struct/PushConstantReserve.hpp
        src/vulkan/configurator/RenderContextConfigurator.hpp
        src/vulkan/configurator/BasicRenderContextConfigurator.hpp
        src/vulkan/hash/DescriptorSetLayoutBuildTypeHasher.hpp
        src/vulkan/context/ImageContext.hpp
        src/vulkan/util/Presenter.hpp
        src/vulkan/configurator/Template1PipelineConfigurator.cpp
        src/vulkan/configurator/Template1PipelineConfigurator.hpp
        src/vulkan/context/DescriptorUpdateContext.hpp
        src/vulkan/util/IndexChecker.hpp
        src/vulkan/util/ImageContextUtils.hpp
        src/vulkan/configurator/GeneralPostProcessPipelineConfigurator.cpp
        src/vulkan/configurator/GeneralPostProcessPipelineConfigurator.hpp
        src/ui/RenderScene.cpp
        src/ui/RenderScene.hpp
        src/ui/Application.cpp
        src/ui/Application.hpp
        src/constants/Win32Constants.hpp
        src/constants/RenderConstants.hpp
        src/constants/NumConstants.hpp
        src/constants/NullConstants.hpp
        src/constants/GraphicsConfigConstants.hpp
        src/constants/TextureFormatConstants.hpp
        src/constants/ExtensionConstants.hpp
        src/constants/StatusConstants.hpp
)


find_package(Vulkan REQUIRED)

target_compile_definitions(RFF
        PRIVATE
        VK_USE_PLATFORM_WIN32_KHR
)

target_include_directories(RFF PRIVATE
        # standard include
        ${CMAKE_SOURCE_DIR}/include
        # extern sources
        ${CMAKE_SOURCE_DIR}/extern
        # vulkan sdk directory
        ${Vulkan_INCLUDE_DIRS}
)

target_link_directories(RFF PRIVATE ${CMAKE_SOURCE_DIR}/lib)

target_link_libraries(RFF PRIVATE
        # OpenGL Linker
        opengl32
        # GMP Linker
        gmp
        # Win32 Linker
        comctl32
        # OpenCV Linkers
        opencv_core4110
        opencv_imgcodecs4110
        opencv_imgproc4110
        opencv_videoio4110
        opencv_highgui4110
        # Vulkan
        Vulkan::Vulkan
)


add_custom_target(spirv ALL COMMAND powershell -ExecutionPolicy Bypass -File "${CMAKE_CURRENT_SOURCE_DIR}/compile.ps1")
add_dependencies(RFF spirv)
