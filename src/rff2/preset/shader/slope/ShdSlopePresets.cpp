//
// Created by Merutilm on 2025-05-28.
//
#include "ShdSlopePresets.hpp"


namespace merutilm::rff2::ShdSlopePresets {

    std::string Disabled::getName() const {
        return "Disabled";
    }

    ShdSlopeSettings Disabled::genSlope() const {
        return ShdSlopeSettings{0, 0, 1.0f, 60, 135};
    }

    std::string NoReflection::getName() const {
        return "No Reflection";
    }

    ShdSlopeSettings NoReflection::genSlope() const {
        return ShdSlopeSettings{300, 0, 1.0f, 60, 135};
    }

    std::string Reflective::getName() const {
        return "Reflective";
    }

    ShdSlopeSettings Reflective::genSlope() const {
        return ShdSlopeSettings{300, 0.5f, 1.0f, 60, 135};
    }


    std::string Translucent::getName() const {
        return "Translucent";
    }

    ShdSlopeSettings Translucent::genSlope() const {
        return ShdSlopeSettings{300, 0, 0.5f, 60, 135};
    }

    std::string Reversed::getName() const {
        return "Reversed";
    }

    ShdSlopeSettings Reversed::genSlope() const {
        return ShdSlopeSettings{-300, 0, 0.5f, 60, 135};
    }

    std::string Micro::getName() const {
        return "Micro";
    }

    ShdSlopeSettings Micro::genSlope() const {
        return ShdSlopeSettings{3, 0, 0.5f, 60, 135};
    }

    std::string Nano::getName() const {
        return "Nano";
    }

    ShdSlopeSettings Nano::genSlope() const {
        return ShdSlopeSettings{0.003f, 0, 0.5f, 60, 135};
    }
}