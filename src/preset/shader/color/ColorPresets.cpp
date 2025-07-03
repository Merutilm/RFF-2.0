//
// Created by Merutilm on 2025-05-28.
//

#include "ColorPresets.h"

namespace merutilm::rff2 {
    std::string ColorPresets::Disabled::getName() const {
        return "Disabled";
    }

    ColorSettings ColorPresets::Disabled::colorSettings() const {
        return ColorSettings{1, 0, 0, 0, 0, 0};
    }

    std::string ColorPresets::WeakContrast::getName() const {
        return "Weak Contrast";
    }

    ColorSettings ColorPresets::WeakContrast::colorSettings() const {
        return ColorSettings{1, 0.1f, 0, 0, 0, 0.1f};
    }

    std::string ColorPresets::HighContrast::getName() const {
        return "High Contrast";
    }

    ColorSettings ColorPresets::HighContrast::colorSettings() const {
        return ColorSettings{1, 0.1f, 0, 0.2f, 0, 0.25f};
    }

    std::string ColorPresets::Dull::getName() const {
        return "Dull";
    }

    ColorSettings ColorPresets::Dull::colorSettings() const {
        return ColorSettings{1, 0.05f, 0, -0.3f, 0, 0.05f};
    }

    std::string ColorPresets::Vivid::getName() const {
        return "Vivid";
    }

    ColorSettings ColorPresets::Vivid::colorSettings() const {
        return ColorSettings{1, 0.2f, 0, 0.5f, 0, 0.05f};
    }
}
