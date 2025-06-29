//
// Created by Merutilm on 2025-05-28.
//
#include "SlopePresets.h"



namespace merutilm::rff2 {

    std::string SlopePresets::Disabled::getName() const {
        return "Disabled";
    }

    SlopeSettings SlopePresets::Disabled::slopeSettings() const {
        return SlopeSettings{0, 0, 1.0f, 60, 135};
    }

    std::string SlopePresets::Normal::getName() const {
        return "Normal";
    }

    SlopeSettings SlopePresets::Normal::slopeSettings() const {
        return SlopeSettings{300, 0, 0.5f, 60, 135};
    }
}