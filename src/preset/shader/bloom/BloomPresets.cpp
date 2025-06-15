//
// Created by Merutilm on 2025-05-28.
//
#include "BloomPresets.h"

std::string merutilm::rff::BloomPresets::Normal::getName() const {
    return "Normal";
}

merutilm::rff::BloomSettings merutilm::rff::BloomPresets::Normal::bloomSettings() const {
    return BloomSettings{0, 0.1f, 0, 1};
}
