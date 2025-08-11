//
// Created by Merutilm on 2025-05-28.
//
#include "FogPresets.h"

namespace merutilm::rff2 {

    std::string FogPresets::Disabled::getName() const {
        return "Disabled";
    }

    FogSettings FogPresets::Disabled::fogSettings() const {
        return FogSettings{0.0, 0.0};
    }

    std::string FogPresets::Low::getName() const {
        return "Low";
    }

    FogSettings FogPresets::Low::fogSettings() const {
        return FogSettings{0.1f, 0.2f};
    }

    std::string FogPresets::Medium::getName() const {
        return "Medium";
    }

    FogSettings FogPresets::Medium::fogSettings() const {
        return FogSettings{0.1f, 0.5f};
    }

    std::string FogPresets::High::getName() const {
        return "High";
    }

    FogSettings FogPresets::High::fogSettings() const {
        return FogSettings{0.15f, 0.8f};

    }

    std::string FogPresets::Ultra::getName() const {
        return "Ultra";
    }

    FogSettings FogPresets::Ultra::fogSettings() const {
        return FogSettings{0.15f, 1};
    }
}
