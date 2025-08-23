//
// Created by Merutilm on 2025-05-28.
//
#include "ShdFogPresets.h"

namespace merutilm::rff2 {

    std::string ShdFogPresets::Disabled::getName() const {
        return "Disabled";
    }

    ShdFogAttribute ShdFogPresets::Disabled::genFog() const {
        return ShdFogAttribute{0.0, 0.0};
    }

    std::string ShdFogPresets::Low::getName() const {
        return "Low";
    }

    ShdFogAttribute ShdFogPresets::Low::genFog() const {
        return ShdFogAttribute{0.1f, 0.2f};
    }

    std::string ShdFogPresets::Medium::getName() const {
        return "Medium";
    }

    ShdFogAttribute ShdFogPresets::Medium::genFog() const {
        return ShdFogAttribute{0.1f, 0.5f};
    }

    std::string ShdFogPresets::High::getName() const {
        return "High";
    }

    ShdFogAttribute ShdFogPresets::High::genFog() const {
        return ShdFogAttribute{0.15f, 0.8f};

    }

    std::string ShdFogPresets::Ultra::getName() const {
        return "Ultra";
    }

    ShdFogAttribute ShdFogPresets::Ultra::genFog() const {
        return ShdFogAttribute{0.15f, 1};
    }
}
