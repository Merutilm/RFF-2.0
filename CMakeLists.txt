cmake_minimum_required(VERSION 3.31)
project(RFF)

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/extern)
link_directories(${CMAKE_SOURCE_DIR}/lib)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY  ${CMAKE_SOURCE_DIR}/bin)

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
        src/merutilm/rff/ui/RFFRenderWindow.cpp
        src/merutilm/rff/ui/RFFRenderWindow.h
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
        src/merutilm/rff/settings/SelectableParser.cpp
        src/merutilm/rff/settings/SelectableParser.h
        src/merutilm/rff/precision/GMPDecimalCalculator.cpp
        src/merutilm/rff/precision/GMPDecimalCalculator.h
        src/merutilm/rff/precision/GMPDecimal.cpp
        src/merutilm/rff/precision/GMPDecimal.h
        src/merutilm/rff/ui/RFFGL.cpp
        src/merutilm/rff/ui/RFFGL.h
        src/merutilm/rff/ui/RFFRenderer.cpp
        src/merutilm/rff/ui/RFFRenderer.h
        src/merutilm/rff/value/PointDouble.cpp
        src/merutilm/rff/value/PointDouble.h
        src/merutilm/rff/calc/Perturbator.cpp
        src/merutilm/rff/calc/Perturbator.h
)
target_link_libraries(RFF PRIVATE opengl32 gmp)
