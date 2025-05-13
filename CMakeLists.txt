cmake_minimum_required(VERSION 3.31)
project(RFF)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/extern)
link_directories(${CMAKE_SOURCE_DIR}/lib)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_SOURCE_DIR}/bin)

add_compile_options(
        -O3
        -ffast-math
        -march=native
        -funroll-loops
)



add_executable(
        RFF
        src/merutilm/rff/ui/RFF.cpp
        extern/glad.c
        extern/glad_wgl.c
        src/merutilm/rff/opengl/GLShaderLoader.cpp
        src/merutilm/rff/opengl/GLRenderer.cpp
        src/merutilm/rff/opengl/GLRenderer.h
        src/merutilm/rff/precision/GMPComplexCalculator.cpp
        src/merutilm/rff/precision/GMPComplexCalculator.h
        src/merutilm/rff/ui/RFFMasterWindow.cpp
        src/merutilm/rff/ui/RFFMasterWindow.h
        src/merutilm/rff/ui/RFFConstants.h
        src/merutilm/rff/settings/Settings.h
        src/merutilm/rff/settings/CalculationSettings.h
        src/merutilm/rff/settings/RenderSettings.h
        src/merutilm/rff/settings/ShaderSettings.h
        src/merutilm/rff/settings/SlopeSettings.h
        src/merutilm/rff/settings/StripeSettings.h
        src/merutilm/rff/settings/ColorSettings.h
        src/merutilm/rff/settings/PaletteSettings.h
        src/merutilm/rff/settings/FogSettings.h
        src/merutilm/rff/settings/VideoSettings.h
        src/merutilm/rff/settings/BloomSettings.h
        src/merutilm/rff/settings/DataSettings.h
        src/merutilm/rff/settings/ExportSettings.h
        src/merutilm/rff/settings/MpaSettings.h
        src/merutilm/rff/precision/Center.cpp
        src/merutilm/rff/precision/Center.h
        src/merutilm/rff/settings/DecimalizeIterationMethod.h
        src/merutilm/rff/settings/ReferenceCompressionSettings.h
        src/merutilm/rff/settings/ReuseReferenceMethod.h
        src/merutilm/rff/settings/Selectable.h
        src/merutilm/rff/precision/GMPDecimalCalculator.cpp
        src/merutilm/rff/precision/GMPDecimalCalculator.h
        src/merutilm/rff/precision/GMPDecimal.cpp
        src/merutilm/rff/precision/GMPDecimal.h
        src/merutilm/rff/ui/RFFGL.cpp
        src/merutilm/rff/ui/RFFGL.h
        src/merutilm/rff/ui/RFFRenderScene.cpp
        src/merutilm/rff/ui/RFFRenderScene.h
        src/merutilm/rff/value/PointDouble.cpp
        src/merutilm/rff/value/PointDouble.h
        src/merutilm/rff/formula/Perturbator.cpp
        src/merutilm/rff/formula/Perturbator.h
        src/merutilm/rff/ui/RFFScene.h
        src/merutilm/rff/ui/RFFScene.cpp
        src/merutilm/rff/io/RFFMap.cpp
        src/merutilm/rff/io/RFFMap.h
        src/merutilm/rff/data/Matrix.h
        src/merutilm/rff/opengl/GLRendererIteration.cpp
        src/merutilm/rff/opengl/GLRendererIteration.h
        src/merutilm/rff/settings/ColorSmoothingSettings.h
        src/merutilm/rff/data/Color.cpp
        src/merutilm/rff/data/Color.h
        src/merutilm/rff/settings/StripeType.h
        src/merutilm/rff/settings/AnimationSettings.h
        src/merutilm/rff/opengl/GLTimeRenderer.h
        src/merutilm/rff/opengl/GLTimeProvider.h
        src/merutilm/rff/formula/LightMandelbrotReference.cpp
        src/merutilm/rff/formula/LightMandelbrotReference.h
        src/merutilm/rff/parallel/ParallelRenderState.cpp
        src/merutilm/rff/parallel/ParallelRenderState.h
        src/merutilm/rff/approx/ApproxMath.h
        src/merutilm/rff/approx/ArrayCompressionTool.h
        src/merutilm/rff/approx/ArrayCompressor.h
        src/merutilm/rff/parallel/ParallelArrayDispatcher.h
        src/merutilm/rff/ui/RFFLogger.h
        src/merutilm/rff/formula/LightMandelbrotPerturbator.cpp
        src/merutilm/rff/formula/LightMandelbrotPerturbator.h
        src/merutilm/rff/approx/LightPA.cpp
        src/merutilm/rff/approx/LightPA.h
        src/merutilm/rff/approx/LightMPATable.cpp
        src/merutilm/rff/approx/LightMPATable.h
        src/merutilm/rff/approx/MPAPeriod.cpp
        src/merutilm/rff/approx/MPAPeriod.h
        src/merutilm/rff/ui/RFFSettingsWindow.cpp
        src/merutilm/rff/ui/RFFSettingsWindow.h
        src/merutilm/rff/ui/RFFSettingsMenu.cpp
        src/merutilm/rff/ui/RFFSettingsMenu.h
        src/merutilm/rff/ui/RFFCallbackFile.cpp
        src/merutilm/rff/ui/RFFCallbackFile.h
        src/merutilm/rff/ui/RFFCallbackFractal.cpp
        src/merutilm/rff/ui/RFFCallbackFractal.h
        src/merutilm/rff/ui/RFFCallbackRender.cpp
        src/merutilm/rff/ui/RFFCallbackRender.h
)
target_link_libraries(RFF PRIVATE opengl32 gmp comctl32)
