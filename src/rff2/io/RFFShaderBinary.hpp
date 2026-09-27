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

        void write(std::ofstream &out) const;
    };
}
