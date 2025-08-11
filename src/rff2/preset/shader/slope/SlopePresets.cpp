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

    std::string SlopePresets::NoReflection::getName() const {
        return "No Reflection";
    }

    SlopeSettings SlopePresets::NoReflection::slopeSettings() const {
        return SlopeSettings{300, 0, 1.0f, 60, 135};
    }

    std::string SlopePresets::Reflective::getName() const {
        return "Reflective";
    }

    SlopeSettings SlopePresets::Reflective::slopeSettings() const {
        return SlopeSettings{300, 0.5f, 1.0f, 60, 135};
    }


    std::string SlopePresets::Translucent::getName() const {
        return "Translucent";
    }

    SlopeSettings SlopePresets::Translucent::slopeSettings() const {
        return SlopeSettings{300, 0, 0.5f, 60, 135};
    }

    std::string SlopePresets::Reversed::getName() const {
        return "Reversed";
    }

    SlopeSettings SlopePresets::Reversed::slopeSettings() const {
        return SlopeSettings{-300, 0, 0.5f, 60, 135};
    }

    std::string SlopePresets::Micro::getName() const {
        return "Micro";
    }

    SlopeSettings SlopePresets::Micro::slopeSettings() const {
        return SlopeSettings{3, 0, 0.5f, 60, 135};
    }

    std::string SlopePresets::Nano::getName() const {
        return "Nano";
    }

    SlopeSettings SlopePresets::Nano::slopeSettings() const {
        return SlopeSettings{0.003f, 0, 0.5f, 60, 135};
    }
}