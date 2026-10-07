//
// Created by Merutilm on 9/27/26.
//

#pragma once
#include "../settings/ShaderSettings.h"
#include "RFFBinary.hpp"

namespace merutilm::rff2 {
    struct RFFShaderBinary : RFFBinary {

        static const RFFShaderBinary DEFAULT;

        ShaderSettings shaderSettings;

        explicit RFFShaderBinary(ShaderSettings shaderSettings);

        [[nodiscard]] static RFFShaderBinary read(std::ifstream &in);
        static void readPaletteSettings(std::ifstream &in, ShdPaletteSettings &palette);
        static void readStripeSettings(std::ifstream &in, ShdStripeSettings &stripe);
        static void readSurfaceSettings(std::ifstream &in, ShdSurfaceSettings &surface, uint32_t version);
        static void readColorSettings(std::ifstream &in, ShdColorSettings &color);
        static void readFogSettings(std::ifstream &in, ShdFogSettings &fog);
        static void readBloomSettings(std::ifstream &in, ShdBloomSettings &bloom);
        static void readNoiseReductionSettings(std::ifstream &in, ShdNoiseReductionSettings &noiseReduction);
        static void readFractal3DSettings(std::ifstream &in, ShdFractal3DSettings &fractal3d);

        void write(std::ofstream &out) const;
    };
}
