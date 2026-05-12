//
// Created by Merutilm on 2025-05-28.
//

#include "ShdColorPresets.h"

namespace merutilm::rff2 {
    std::wstring ShdColorPresets::Disabled::getName() const {
        return L"Disabled";
    }

    ShdColorSettings ShdColorPresets::Disabled::genColor() const {
        return ShdColorSettings{1, 0, 0, 0, 0, 0};
    }

    std::wstring ShdColorPresets::WeakContrast::getName() const {
        return L"Weak Contrast";
    }

    ShdColorSettings ShdColorPresets::WeakContrast::genColor() const {
        return ShdColorSettings{1, 0.1f, 0, 0, 0, 0.1f};
    }

    std::wstring ShdColorPresets::HighContrast::getName() const {
        return L"High Contrast";
    }

    ShdColorSettings ShdColorPresets::HighContrast::genColor() const {
        return ShdColorSettings{1, 0.1f, 0, 0.2f, 0, 0.25f};
    }

    std::wstring ShdColorPresets::Dull::getName() const {
        return L"Dull";
    }

    ShdColorSettings ShdColorPresets::Dull::genColor() const {
        return ShdColorSettings{1, 0.05f, 0, -0.3f, 0, 0.05f};
    }

    std::wstring ShdColorPresets::Vivid::getName() const {
        return L"Vivid";
    }

    ShdColorSettings ShdColorPresets::Vivid::genColor() const {
        return ShdColorSettings{1, 0.2f, 0, 0.5f, 0, 0.05f};
    }
}
