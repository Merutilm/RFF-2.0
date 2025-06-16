//
// Created by Merutilm on 2025-05-28.
//
#include "FogPresets.h"

namespace merutilm::rff {
    std::string FogPresets::Medium::getName() const {
        return "Medium";
    }

    FogSettings FogPresets::Medium::fogSettings() const {
        return FogSettings{0.1, 0.5};
    }
}