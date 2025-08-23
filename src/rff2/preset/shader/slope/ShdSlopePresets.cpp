//
// Created by Merutilm on 2025-05-28.
//
#include "ShdSlopePresets.h"



namespace merutilm::rff2 {

    std::string ShdSlopePresets::Disabled::getName() const {
        return "Disabled";
    }

    ShdSlopeAttribute ShdSlopePresets::Disabled::genSlope() const {
        return ShdSlopeAttribute{0, 0, 1.0f, 60, 135};
    }

    std::string ShdSlopePresets::NoReflection::getName() const {
        return "No Reflection";
    }

    ShdSlopeAttribute ShdSlopePresets::NoReflection::genSlope() const {
        return ShdSlopeAttribute{300, 0, 1.0f, 60, 135};
    }

    std::string ShdSlopePresets::Reflective::getName() const {
        return "Reflective";
    }

    ShdSlopeAttribute ShdSlopePresets::Reflective::genSlope() const {
        return ShdSlopeAttribute{300, 0.5f, 1.0f, 60, 135};
    }


    std::string ShdSlopePresets::Translucent::getName() const {
        return "Translucent";
    }

    ShdSlopeAttribute ShdSlopePresets::Translucent::genSlope() const {
        return ShdSlopeAttribute{300, 0, 0.5f, 60, 135};
    }

    std::string ShdSlopePresets::Reversed::getName() const {
        return "Reversed";
    }

    ShdSlopeAttribute ShdSlopePresets::Reversed::genSlope() const {
        return ShdSlopeAttribute{-300, 0, 0.5f, 60, 135};
    }

    std::string ShdSlopePresets::Micro::getName() const {
        return "Micro";
    }

    ShdSlopeAttribute ShdSlopePresets::Micro::genSlope() const {
        return ShdSlopeAttribute{3, 0, 0.5f, 60, 135};
    }

    std::string ShdSlopePresets::Nano::getName() const {
        return "Nano";
    }

    ShdSlopeAttribute ShdSlopePresets::Nano::genSlope() const {
        return ShdSlopeAttribute{0.003f, 0, 0.5f, 60, 135};
    }
}