//
// Created by Merutilm on 2025-05-31.
//

#include "CalculationPresets.h"


std::string CalculationPresets::UltraFast::getName() const {
    return "Ultra Fast";
}

MPASettings CalculationPresets::UltraFast::mpaSettings() const {
    return MPASettings{4, 2, -3, MPASelectionMethod::HIGHEST, MPACompressionMethod::NO_COMPRESSION};
}

ReferenceCompressionSettings CalculationPresets::UltraFast::referenceCompressionSettings() const {
    return ReferenceCompressionSettings{0, 0, false};
}

std::string CalculationPresets::Fast::getName() const {
    return "Fast";
}

MPASettings CalculationPresets::Fast::mpaSettings() const {
    return MPASettings{8, 2, -4, MPASelectionMethod::HIGHEST, MPACompressionMethod::NO_COMPRESSION};
}

ReferenceCompressionSettings CalculationPresets::Fast::referenceCompressionSettings() const {
    return ReferenceCompressionSettings{1000000, 7, false};
}

std::string CalculationPresets::Normal::getName() const {
    return "Normal";
}

MPASettings CalculationPresets::Normal::mpaSettings() const {
    return MPASettings{8, 2, -5, MPASelectionMethod::HIGHEST, MPACompressionMethod::LITTLE_COMPRESSION};
}

ReferenceCompressionSettings CalculationPresets::Normal::referenceCompressionSettings() const {
    return ReferenceCompressionSettings{1000000, 11, false};
}

std::string CalculationPresets::Best::getName() const {
    return "Best";
}

MPASettings CalculationPresets::Best::mpaSettings() const {
    return MPASettings{8, 2, -6, MPASelectionMethod::HIGHEST, MPACompressionMethod::LITTLE_COMPRESSION};
}

ReferenceCompressionSettings CalculationPresets::Best::referenceCompressionSettings() const {
    return ReferenceCompressionSettings{1000000, 15, false};
}

std::string CalculationPresets::UltraBest::getName() const {
    return "Ultra Best";
}

MPASettings CalculationPresets::UltraBest::mpaSettings() const {
    return MPASettings{8, 2, -7, MPASelectionMethod::HIGHEST, MPACompressionMethod::LITTLE_COMPRESSION};
}

ReferenceCompressionSettings CalculationPresets::UltraBest::referenceCompressionSettings() const {
    return ReferenceCompressionSettings{1000000, 19, false};
}

std::string CalculationPresets::Stable::getName() const {
    return "Stable";
}

MPASettings CalculationPresets::Stable::mpaSettings() const {
    return MPASettings{8, 2, -4, MPASelectionMethod::HIGHEST, MPACompressionMethod::STRONGEST};
}

ReferenceCompressionSettings CalculationPresets::Stable::referenceCompressionSettings() const {
    return ReferenceCompressionSettings{1000000, 6, false};
}

std::string CalculationPresets::MoreStable::getName() const {
    return "More Stable";
}

MPASettings CalculationPresets::MoreStable::mpaSettings() const {
    return MPASettings{8, 2, -4, MPASelectionMethod::HIGHEST, MPACompressionMethod::STRONGEST};
}

ReferenceCompressionSettings CalculationPresets::MoreStable::referenceCompressionSettings() const {
    return ReferenceCompressionSettings{100000, 6, false};
}

std::string CalculationPresets::UltraStable::getName() const {
    return "Ultra Stable";
}

MPASettings CalculationPresets::UltraStable::mpaSettings() const {
    return MPASettings{8, 2, -4, MPASelectionMethod::HIGHEST, MPACompressionMethod::STRONGEST};
}

ReferenceCompressionSettings CalculationPresets::UltraStable::referenceCompressionSettings() const {
    return ReferenceCompressionSettings{10000, 6, true};
}
