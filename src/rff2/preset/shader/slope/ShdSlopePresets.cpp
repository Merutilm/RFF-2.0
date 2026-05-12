//
// Created by Merutilm on 2025-05-28.
//
#include "ShdSlopePresets.h"



namespace merutilm::rff2 {

    std::wstring ShdSlopePresets::Disabled::getName() const {
        return L"Disabled";
    }

    ShdSlopeSettings ShdSlopePresets::Disabled::genSlope() const {
        return ShdSlopeSettings{0, 0, 1.0f, 60, 135};
    }

    std::wstring ShdSlopePresets::NoReflection::getName() const {
        return L"No Reflection";
    }

    ShdSlopeSettings ShdSlopePresets::NoReflection::genSlope() const {
        return ShdSlopeSettings{300, 0, 1.0f, 60, 135};
    }

    std::wstring ShdSlopePresets::Reflective::getName() const {
        return L"Reflective";
    }

    ShdSlopeSettings ShdSlopePresets::Reflective::genSlope() const {
        return ShdSlopeSettings{300, 0.5f, 1.0f, 60, 135};
    }


    std::wstring ShdSlopePresets::Translucent::getName() const {
        return L"Translucent";
    }

    ShdSlopeSettings ShdSlopePresets::Translucent::genSlope() const {
        return ShdSlopeSettings{300, 0, 0.5f, 60, 135};
    }

    std::wstring ShdSlopePresets::Reversed::getName() const {
        return L"Reversed";
    }

    ShdSlopeSettings ShdSlopePresets::Reversed::genSlope() const {
        return ShdSlopeSettings{-300, 0, 0.5f, 60, 135};
    }

    std::wstring ShdSlopePresets::Micro::getName() const {
        return L"Micro";
    }

    ShdSlopeSettings ShdSlopePresets::Micro::genSlope() const {
        return ShdSlopeSettings{3, 0, 0.5f, 60, 135};
    }

    std::wstring ShdSlopePresets::Nano::getName() const {
        return L"Nano";
    }

    ShdSlopeSettings ShdSlopePresets::Nano::genSlope() const {
        return ShdSlopeSettings{0.003f, 0, 0.5f, 60, 135};
    }
}