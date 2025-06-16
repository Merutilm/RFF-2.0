//
// Created by Merutilm on 2025-05-28.
//
#include "SlopePresets.h"



namespace merutilm::rff {
    std::string SlopePresets::Normal::getName() const {
        return "Normal";
    }

    SlopeSettings SlopePresets::Normal::slopeSettings() const {
        return SlopeSettings{300, 0.5f, 1, 60, 135};
    }
}