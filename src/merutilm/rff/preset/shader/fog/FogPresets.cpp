//
// Created by Merutilm on 2025-05-28.
//
#include "FogPresets.h"

std::string merutilm::rff::FogPresets::Medium::getName() const {
    return "Medium";
}

merutilm::rff::FogSettings merutilm::rff::FogPresets::Medium::fogSettings() const {
    return FogSettings{0.1, 0.5};
}
