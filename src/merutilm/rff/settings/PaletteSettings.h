#pragma once
#include <vector>

#include "ColorSmoothingSettings.h"
#include "../data/ColorFloat.h"

namespace merutilm::rff {
    struct PaletteSettings {
        std::vector<ColorFloat> colors;
        ColorSmoothingSettings colorSmoothing;
        float iterationInterval;
        float offsetRatio;
        float animationSpeed;

        ColorFloat getMidColor(float rat) const;
    };

    inline ColorFloat PaletteSettings::getMidColor(const float rat) const {
        const float ratio = std::fmod(rat / iterationInterval + offsetRatio, 1);
        const float i = ratio * colors.size();
        const int i0 = static_cast<int>(i);
        const int i1 = i0 + 1;

        const ColorFloat &c1 = colors[i0 % colors.size()];
        const ColorFloat &c2 = colors[i1 % colors.size()];
        return ColorFloat::lerp(c1, c2, std::fmod(i, 1));
    }
}