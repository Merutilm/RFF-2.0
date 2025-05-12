
#pragma once
#include <glad.h>
#include <array>
#include <cstdint>
#include <string>
#include <vector>

#include "../approx/LightPA.h"

using TextureFormat = std::array<GLuint, 3>;

namespace RFFConstants {
    namespace Win32 {
        constexpr short INIT_RENDER_SCENE_WIDTH = 1280;
        constexpr short INIT_RENDER_SCENE_HEIGHT = 720;
        constexpr int INIT_RENDER_SCENE_FPS = 60;
        constexpr auto CLASS_MASTER_WINDOW = "RFF 2.0";
        constexpr auto CLASS_RENDER_SCENE = "RFF 2.0 Renderer";
        constexpr auto CLASS_SETTINGS = "RFF Settings";
    }


    namespace Render {
        constexpr int EXIT_CHECK_INTERVAL = 256;
        constexpr float ZOOM_MIN = 1.0f;
        constexpr float ZOOM_INTERVAL = 0.235f;
        constexpr uint64_t MINIMUM_ITERATION = 3000;
        constexpr int AUTOMATIC_ITERATION_MULTIPLIER = 50;
    }

    namespace Precision {
        constexpr float LOG10_2 = 0.3010299956639811952137388947244930267681898814621085375f;
        constexpr int DOUBLE_PRECISION = 52;
        constexpr uint64_t SIGNUM_BIT = 0x8000000000000000LL;
        constexpr uint64_t EXP0_BITS = 0x3ff0000000000000LL;
        constexpr uint64_t DECIMAL_SIGNUM_BITS = 0x800fffffffffffffLL;
        constexpr int PRECISION_ADDITION = 15;
    }
    namespace TextureFormats {
        constexpr int INTERNAL = 0;
        constexpr int FORMAT = 1;
        constexpr int TYPE = 2;
        constexpr TextureFormat INT1 = {GL_R32I, GL_RED_INTEGER, GL_INT};
        constexpr TextureFormat INT2 = {GL_RG32I, GL_RG_INTEGER, GL_INT};
        constexpr TextureFormat INT3 = {GL_RGB32I, GL_RGB_INTEGER, GL_INT};
        constexpr TextureFormat INT4 = {GL_RGBA32I, GL_RGBA_INTEGER, GL_INT};
        constexpr TextureFormat FLOAT1 = {GL_R32F, GL_RED, GL_FLOAT};
        constexpr TextureFormat FLOAT2 = {GL_RG32F, GL_RG, GL_FLOAT};
        constexpr TextureFormat FLOAT3 = {GL_RGB32F, GL_RGB, GL_FLOAT};
        constexpr TextureFormat FLOAT4 = {GL_RGBA32F, GL_RGBA, GL_FLOAT};
    }

    namespace Status {
        constexpr int ITERATION_STATUS = 0;
        constexpr int ZOOM_STATUS = 1;
        constexpr int PERIOD_STATUS = 2;
        constexpr int TIME_STATUS = 3;
        constexpr int RENDER_STATUS = 4;
        constexpr int LENGTH = 5;
        constexpr int SET_PROCESS_INTERVAL_MS = 10;
    }
    namespace Parser {
        constexpr auto UNKNOWN = "UNKNOWN";
    }

    namespace Approximation {
        constexpr int REQUIRED_PERTURBATION = 2;
    }
    namespace GLConfig {
        constexpr auto SHADER_PATH_PREFIX = "../shaders/";
        constexpr auto VERTEX_PATH_DEFAULT = "default_vertex";
        constexpr auto VERTEX_PATH_SUFFIX = ".vert";
        constexpr auto FRAGMENT_PATH_SUFFIX = ".frag";
        constexpr auto MESSAGE_CANNOT_OPEN_FILE = "Error: Could not open file: ";
    }

}
