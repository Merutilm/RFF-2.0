#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "ShdPalColorSmoothingMethod.h"


namespace merutilm::rff2 {
    struct ShdPaletteAttribute {
        std::vector<glm::vec4> colors;
        ShdPalColorSmoothingMethod colorSmoothing;
        float iterationInterval;
        float offsetRatio;
        float animationSpeed;

        glm::vec4 getMidColor(float rat) const;
    };

    inline glm::vec4 ShdPaletteAttribute::getMidColor(const float rat) const {
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
