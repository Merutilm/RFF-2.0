//
// Created by Merutilm on 2025-05-28.
//
#include "ShdSlopePresets.h"



namespace merutilm::rff2 {

    std::string ShdSlopePresets::Disabled::getName() const {
        return "Disabled";
    }

    ShdSlopeSettings ShdSlopePresets::Disabled::genSlope() const {
        return ShdSlopeSettings{0, 0, 1.0f, 60, 135};
    }

    std::string ShdSlopePresets::NoReflection::getName() const {
        return "No Reflection";
    }

    ShdSlopeSettings ShdSlopePresets::NoReflection::genSlope() const {
        return ShdSlopeSettings{300, 0, 1.0f, 60, 135};
    }

    std::string ShdSlopePresets::Reflective::getName() const {
        return "Reflective";
    }

    ShdSlopeSettings ShdSlopePresets::Reflective::genSlope() const {
        return ShdSlopeSettings{300, 0.5f, 1.0f, 60, 135};
    }


    std::string ShdSlopePresets::Translucent::getName() const {
        return "Translucent";
    }

    ShdSlopeSettings ShdSlopePresets::Translucent::genSlope() const {
        return ShdSlopeSettings{300, 0, 0.5f, 60, 135};
    }

    std::string ShdSlopePresets::Reversed::getName() const {
        return "Reversed";
    }

    ShdSlopeSettings ShdSlopePresets::Reversed::genSlope() const {
        return ShdSlopeSettings{-300, 0, 0.5f, 60, 135};
    }

    std::string ShdSlopePresets::Micro::getName() const {
        return "Micro";
    }

    ShdSlopeSettings ShdSlopePresets::Micro::genSlope() const {
        return ShdSlopeSettings{3, 0, 0.5f, 60, 135};
    }

    std::string ShdSlopePresets::Nano::getName() const {
        return "Nano";
    }

    ShdSlopeSettings ShdSlopePresets::Nano::genSlope() const {
        return ShdSlopeSettings{0.003f, 0, 0.5f, 60, 135};
    }
}