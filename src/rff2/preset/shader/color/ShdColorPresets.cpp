//
// Created by Merutilm on 2025-05-28.
//

#include "ShdColorPresets.h"

namespace merutilm::rff2 {
    std::string ShdColorPresets::Disabled::getName() const {
        return "Disabled";
    }

    ShdColorAttribute ShdColorPresets::Disabled::genColor() const {
        return ShdColorAttribute{1, 0, 0, 0, 0, 0};
    }

    std::string ShdColorPresets::WeakContrast::getName() const {
        return "Weak Contrast";
    }

    ShdColorAttribute ShdColorPresets::WeakContrast::genColor() const {
        return ShdColorAttribute{1, 0.1f, 0, 0, 0, 0.1f};
    }

    std::string ShdColorPresets::HighContrast::getName() const {
        return "High Contrast";
    }

    ShdColorAttribute ShdColorPresets::HighContrast::genColor() const {
        return ShdColorAttribute{1, 0.1f, 0, 0.2f, 0, 0.25f};
    }

    std::string ShdColorPresets::Dull::getName() const {
        return "Dull";
    }

    ShdColorAttribute ShdColorPresets::Dull::genColor() const {
        return ShdColorAttribute{1, 0.05f, 0, -0.3f, 0, 0.05f};
    }

    std::string ShdColorPresets::Vivid::getName() const {
        return "Vivid";
    }

    ShdColorAttribute ShdColorPresets::Vivid::genColor() const {
        return ShdColorAttribute{1, 0.2f, 0, 0.5f, 0, 0.05f};
    }
}
