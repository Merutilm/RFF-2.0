//
// Created by Merutilm on 2025-05-28.
//
#include "ShdFogPresets.h"

namespace merutilm::rff2 {

    std::wstring ShdFogPresets::Disabled::getName() const {
        return L"Disabled";
    }

    ShdFogSettings ShdFogPresets::Disabled::genFog() const {
        return ShdFogSettings{0.0, 0.0};
    }

    std::wstring ShdFogPresets::Low::getName() const {
        return L"Low";
    }

    ShdFogSettings ShdFogPresets::Low::genFog() const {
        return ShdFogSettings{0.1f, 0.2f};
    }

    std::wstring ShdFogPresets::Medium::getName() const {
        return L"Medium";
    }

    ShdFogSettings ShdFogPresets::Medium::genFog() const {
        return ShdFogSettings{0.1f, 0.5f};
    }

    std::wstring ShdFogPresets::High::getName() const {
        return L"High";
    }

    ShdFogSettings ShdFogPresets::High::genFog() const {
        return ShdFogSettings{0.15f, 0.8f};

    }

    std::wstring ShdFogPresets::Ultra::getName() const {
        return L"Ultra";
    }

    ShdFogSettings ShdFogPresets::Ultra::genFog() const {
        return ShdFogSettings{0.15f, 1};
    }
}
