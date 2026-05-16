#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "ShdIterationColoringMethod.hpp"
#include "ShdPalSingleIterationColoringMethod.h"


namespace merutilm::rff2 {
    struct ShdPaletteSettings {
        std::vector<glm::vec4> colors;
        ShdIterationColoringMethod iterationColoring;
        ShdPalSingleIterationColoringMethod singleIterationColoring;
        float iterationInterval;
        float offsetRatio;
        float animationSpeed;

        glm::vec4 getMidColor(float rat) const;
    };

    inline glm::vec4 ShdPaletteSettings::getMidColor(const float rat) const {
        const float ratio = std::fmod(rat / iterationInterval + offsetRatio, 1.0f);
        const float i = ratio * static_cast<float>(colors.size());
        const auto i0 = static_cast<int>(i);
        const auto i1 = i0 + 1;

        const glm::vec4 &c1 = colors[i0 % colors.size()];
        const glm::vec4 &c2 = colors[i1 % colors.size()];
        const float rd = std::fmod(i, 1.0f);
        return glm::vec4{
            std::lerp(c1.r, c2.r, rd),
            std::lerp(c1.g, c2.g, rd),
            std::lerp(c1.b, c2.b, rd),
            std::lerp(c1.a, c2.a, rd)
        };
    }
}
