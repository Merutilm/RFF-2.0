#pragma once
#include <vector>

#include "ColorSmoothingSettings.h"
#include "../data/Color.h"

struct PaletteSettings {
    std::vector<Color> colors;
    ColorSmoothingSettings colorSmoothing;
    float iterationInterval;
    float offsetRatio;
    float animationSpeed;
};
