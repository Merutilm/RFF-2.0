//
// Created by Merutilm on 2025-05-28.
//

#include "ShdColorPresets.hpp"

namespace merutilm::rff2::ShdColorPresets {
    std::string Disabled::getName() const {
        return "Disabled";
    }

    ShdColorSettings Disabled::genColor() const {
        return ShdColorSettings{1, 0, 0, 0, 0, 0};
    }

    std::string WeakContrast::getName() const {
        return "Weak Contrast";
    }

    ShdColorSettings WeakContrast::genColor() const {
        return ShdColorSettings{1, 0.1f, 0, 0, 0, 0.1f};
    }

    std::string HighContrast::getName() const {
        return "High Contrast";
    }

    ShdColorSettings HighContrast::genColor() const {
        return ShdColorSettings{1, 0.1f, 0, 0.2f, 0, 0.25f};
    }

    std::string Dull::getName() const {
        return "Dull";
    }

    ShdColorSettings Dull::genColor() const {
        return ShdColorSettings{1, 0.05f, 0, -0.3f, 0, 0.05f};
    }

    std::string Vivid::getName() const {
        return "Vivid";
    }

    ShdColorSettings Vivid::genColor() const {
        return ShdColorSettings{1, 0.2f, 0, 0.5f, 0, 0.05f};
    }
}
