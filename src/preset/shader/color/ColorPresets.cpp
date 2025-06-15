//
// Created by Merutilm on 2025-05-28.
//

#include "ColorPresets.h"

std::string merutilm::rff::ColorPresets::WeakContrast::getName() const {
    return "Weak Contrast";
}

merutilm::rff::ColorSettings merutilm::rff::ColorPresets::WeakContrast::colorSettings() const {
    return ColorSettings{1, 0.1f, 0, 0, 0, 0.1f};
}