//
// Created by Merutilm on 2025-05-28.
//
#include "ShdFogPresets.h"

namespace merutilm::rff2 {

    std::string ShdFogPresets::Disabled::getName() const {
        return "Disabled";
    }

    ShdFogSettings ShdFogPresets::Disabled::genFog() const {
        return ShdFogSettings{0.0, 0.0};
    }

    std::string ShdFogPresets::Low::getName() const {
        return "Low";
    }

    ShdFogSettings ShdFogPresets::Low::genFog() const {
        return ShdFogSettings{0.1f, 0.2f};
    }

    std::string ShdFogPresets::Medium::getName() const {
        return "Medium";
    }

    ShdFogSettings ShdFogPresets::Medium::genFog() const {
        return ShdFogSettings{0.1f, 0.5f};
    }

    std::string ShdFogPresets::High::getName() const {
        return "High";
    }

    ShdFogSettings ShdFogPresets::High::genFog() const {
        return ShdFogSettings{0.15f, 0.8f};

    }

    std::string ShdFogPresets::Ultra::getName() const {
        return "Ultra";
    }

    ShdFogSettings ShdFogPresets::Ultra::genFog() const {
        return ShdFogSettings{0.15f, 1};
    }
}
