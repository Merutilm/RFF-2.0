//
// Created by Merutilm on 2025-05-28.
//

#include "ColorPresets.h"

namespace merutilm::rff2 {
    std::string ColorPresets::WeakContrast::getName() const {
        return "Weak Contrast";
    }

    ColorSettings ColorPresets::WeakContrast::colorSettings() const {
        return ColorSettings{1, 0.1f, 0, 0, 0, 0.1f};
    }
}