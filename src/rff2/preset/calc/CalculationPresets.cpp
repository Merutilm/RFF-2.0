//
// Created by Merutilm on 2025-05-31.
//

#include "CalculationPresets.h"


namespace merutilm::rff2 {
    std::string CalculationPresets::UltraFast::getName() const {
        return "Ultra Fast";
    }

    FrtMPAAttribute CalculationPresets::UltraFast::genMPA() const {
        return FrtMPAAttribute{4, 2, -3, FrtMPASelectionMethod::HIGHEST, FrtMPACompressionMethod::NO_COMPRESSION};
    }

    FrtReferenceCompAttribute CalculationPresets::UltraFast::genReferenceCompression() const {
        return FrtReferenceCompAttribute{0, 0, false};
    }

    std::string CalculationPresets::Fast::getName() const {
        return "Fast";
    }

    FrtMPAAttribute CalculationPresets::Fast::genMPA() const {
        return FrtMPAAttribute{8, 2, -4, FrtMPASelectionMethod::HIGHEST, FrtMPACompressionMethod::NO_COMPRESSION};
    }

    FrtReferenceCompAttribute CalculationPresets::Fast::genReferenceCompression() const {
        return FrtReferenceCompAttribute{1000000, 7, false};
    }

    std::string CalculationPresets::Normal::getName() const {
        return "Normal";
    }

    FrtMPAAttribute CalculationPresets::Normal::genMPA() const {
        return FrtMPAAttribute{8, 2, -5, FrtMPASelectionMethod::HIGHEST, FrtMPACompressionMethod::LITTLE_COMPRESSION};
    }

    FrtReferenceCompAttribute CalculationPresets::Normal::genReferenceCompression() const {
        return FrtReferenceCompAttribute{1000000, 11, false};
    }

    std::string CalculationPresets::Best::getName() const {
        return "Best";
    }

    FrtMPAAttribute CalculationPresets::Best::genMPA() const {
        return FrtMPAAttribute{8, 2, -6, FrtMPASelectionMethod::HIGHEST, FrtMPACompressionMethod::LITTLE_COMPRESSION};
    }

    FrtReferenceCompAttribute CalculationPresets::Best::genReferenceCompression() const {
        return FrtReferenceCompAttribute{1000000, 15, false};
    }

    std::string CalculationPresets::UltraBest::getName() const {
        return "Ultra Best";
    }

    FrtMPAAttribute CalculationPresets::UltraBest::genMPA() const {
        return FrtMPAAttribute{8, 2, -7, FrtMPASelectionMethod::HIGHEST, FrtMPACompressionMethod::LITTLE_COMPRESSION};
    }

    FrtReferenceCompAttribute CalculationPresets::UltraBest::genReferenceCompression() const {
        return FrtReferenceCompAttribute{1000000, 19, false};
    }

    std::string CalculationPresets::Stable::getName() const {
        return "Stable";
    }

    FrtMPAAttribute CalculationPresets::Stable::genMPA() const {
        return FrtMPAAttribute{8, 2, -4, FrtMPASelectionMethod::HIGHEST, FrtMPACompressionMethod::STRONGEST};
    }

    FrtReferenceCompAttribute CalculationPresets::Stable::genReferenceCompression() const {
        return FrtReferenceCompAttribute{1000000, 6, false};
    }

    std::string CalculationPresets::MoreStable::getName() const {
        return "More Stable";
    }

    FrtMPAAttribute CalculationPresets::MoreStable::genMPA() const {
        return FrtMPAAttribute{8, 2, -4, FrtMPASelectionMethod::HIGHEST, FrtMPACompressionMethod::STRONGEST};
    }

    FrtReferenceCompAttribute CalculationPresets::MoreStable::genReferenceCompression() const {
        return FrtReferenceCompAttribute{100000, 6, false};
    }

    std::string CalculationPresets::UltraStable::getName() const {
        return "Ultra Stable";
    }

    FrtMPAAttribute CalculationPresets::UltraStable::genMPA() const {
        return FrtMPAAttribute{8, 2, -4, FrtMPASelectionMethod::HIGHEST, FrtMPACompressionMethod::STRONGEST};
    }

    FrtReferenceCompAttribute CalculationPresets::UltraStable::genReferenceCompression() const {
        return FrtReferenceCompAttribute{10000, 6, true};
    }
}