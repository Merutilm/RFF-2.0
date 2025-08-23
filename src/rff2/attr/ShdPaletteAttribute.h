#pragma once
#include <vector>

#include "ColorSmoothingMethod.h"
#include "../data/NormalizedColor.h"

namespace merutilm::rff2 {
    struct ShdPaletteAttribute {
        std::vector<NormalizedColor> colors;
        ColorSmoothingMethod colorSmoothing;
        float iterationInterval;
        float offsetRatio;
        float animationSpeed;

        NormalizedColor getMidColor(float rat) const;
    };

    inline NormalizedColor ShdPaletteAttribute::getMidColor(const float rat) const {
        const float ratio = std::fmod(rat / iterationInterval + offsetRatio, 1);
        const float i = ratio * colors.size();
        const int i0 = static_cast<int>(i);
        const int i1 = i0 + 1;

        const NormalizedColor &c1 = colors[i0 % colors.size()];
        const NormalizedColor &c2 = colors[i1 % colors.size()];
        return NormalizedColor::lerp(c1, c2, std::fmod(i, 1));
    }
}