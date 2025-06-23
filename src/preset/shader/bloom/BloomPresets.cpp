//
// Created by Merutilm on 2025-05-28.
//
#include "BloomPresets.h"

namespace merutilm::rff {

    std::string BloomPresets::Disabled::getName() const {
        return "Disabled";
    }

    BloomSettings BloomPresets::Disabled::bloomSettings() const {
        return BloomSettings{0, 0.0f, 0, 0};
    }

    std::string BloomPresets::Normal::getName() const {
        return "Normal";
    }

    BloomSettings BloomPresets::Normal::bloomSettings() const {
        return BloomSettings{0, 0.1f, 0, 1};
    }
}