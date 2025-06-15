//
// Created by Merutilm on 2025-05-28.
//
#include "SlopePresets.h"



std::string merutilm::rff::SlopePresets::Normal::getName() const {
    return "Normal";
}

merutilm::rff::SlopeSettings merutilm::rff::SlopePresets::Normal::slopeSettings() const {
    return SlopeSettings{300, 0.5f, 1, 60, 135};
}
