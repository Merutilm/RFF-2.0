//
// Created by Merutilm on 2025-05-31.
//

#include "CalculationPresets.h"


std::string merutilm::rff::CalculationPresets::UltraFast::getName() const {
    return "Ultra Fast";
}

merutilm::rff::MPASettings merutilm::rff::CalculationPresets::UltraFast::mpaSettings() const {
    return MPASettings{4, 2, -3, MPASelectionMethod::HIGHEST, MPACompressionMethod::NO_COMPRESSION};
}

merutilm::rff::ReferenceCompressionSettings merutilm::rff::CalculationPresets::UltraFast::referenceCompressionSettings() const {
    return ReferenceCompressionSettings{0, 0, false};
}

std::string merutilm::rff::CalculationPresets::Fast::getName() const {
    return "Fast";
}

merutilm::rff::MPASettings merutilm::rff::CalculationPresets::Fast::mpaSettings() const {
    return MPASettings{8, 2, -4, MPASelectionMethod::HIGHEST, MPACompressionMethod::NO_COMPRESSION};
}

merutilm::rff::ReferenceCompressionSettings merutilm::rff::CalculationPresets::Fast::referenceCompressionSettings() const {
    return ReferenceCompressionSettings{1000000, 7, false};
}

std::string merutilm::rff::CalculationPresets::Normal::getName() const {
    return "Normal";
}

merutilm::rff::MPASettings merutilm::rff::CalculationPresets::Normal::mpaSettings() const {
    return MPASettings{8, 2, -5, MPASelectionMethod::HIGHEST, MPACompressionMethod::LITTLE_COMPRESSION};
}

merutilm::rff::ReferenceCompressionSettings merutilm::rff::CalculationPresets::Normal::referenceCompressionSettings() const {
    return ReferenceCompressionSettings{1000000, 11, false};
}

std::string merutilm::rff::CalculationPresets::Best::getName() const {
    return "Best";
}

merutilm::rff::MPASettings merutilm::rff::CalculationPresets::Best::mpaSettings() const {
    return MPASettings{8, 2, -6, MPASelectionMethod::HIGHEST, MPACompressionMethod::LITTLE_COMPRESSION};
}

merutilm::rff::ReferenceCompressionSettings merutilm::rff::CalculationPresets::Best::referenceCompressionSettings() const {
    return ReferenceCompressionSettings{1000000, 15, false};
}

std::string merutilm::rff::CalculationPresets::UltraBest::getName() const {
    return "Ultra Best";
}

merutilm::rff::MPASettings merutilm::rff::CalculationPresets::UltraBest::mpaSettings() const {
    return MPASettings{8, 2, -7, MPASelectionMethod::HIGHEST, MPACompressionMethod::LITTLE_COMPRESSION};
}

merutilm::rff::ReferenceCompressionSettings merutilm::rff::CalculationPresets::UltraBest::referenceCompressionSettings() const {
    return ReferenceCompressionSettings{1000000, 19, false};
}

std::string merutilm::rff::CalculationPresets::Stable::getName() const {
    return "Stable";
}

merutilm::rff::MPASettings merutilm::rff::CalculationPresets::Stable::mpaSettings() const {
    return MPASettings{8, 2, -4, MPASelectionMethod::HIGHEST, MPACompressionMethod::STRONGEST};
}

merutilm::rff::ReferenceCompressionSettings merutilm::rff::CalculationPresets::Stable::referenceCompressionSettings() const {
    return ReferenceCompressionSettings{1000000, 6, false};
}

std::string merutilm::rff::CalculationPresets::MoreStable::getName() const {
    return "More Stable";
}

merutilm::rff::MPASettings merutilm::rff::CalculationPresets::MoreStable::mpaSettings() const {
    return MPASettings{8, 2, -4, MPASelectionMethod::HIGHEST, MPACompressionMethod::STRONGEST};
}

merutilm::rff::ReferenceCompressionSettings merutilm::rff::CalculationPresets::MoreStable::referenceCompressionSettings() const {
    return ReferenceCompressionSettings{100000, 6, false};
}

std::string merutilm::rff::CalculationPresets::UltraStable::getName() const {
    return "Ultra Stable";
}

merutilm::rff::MPASettings merutilm::rff::CalculationPresets::UltraStable::mpaSettings() const {
    return MPASettings{8, 2, -4, MPASelectionMethod::HIGHEST, MPACompressionMethod::STRONGEST};
}

merutilm::rff::ReferenceCompressionSettings merutilm::rff::CalculationPresets::UltraStable::referenceCompressionSettings() const {
    return ReferenceCompressionSettings{10000, 6, true};
}
