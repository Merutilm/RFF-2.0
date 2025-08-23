//
// Created by Merutilm on 2025-05-31.
//

#include "CalculationPresets.h"


namespace merutilm::rff2 {
    std::string CalculationPresets::UltraFast::getName() const {
        return "Ultra Fast";
    }

    CalMPAAttribute CalculationPresets::UltraFast::genMPA() const {
        return CalMPAAttribute{4, 2, -3, CalMPASelectionMethod::HIGHEST, CalMPACompressionMethod::NO_COMPRESSION};
    }

    CalReferenceCompAttribute CalculationPresets::UltraFast::genReferenceCompression() const {
        return CalReferenceCompAttribute{0, 0, false};
    }

    std::string CalculationPresets::Fast::getName() const {
        return "Fast";
    }

    CalMPAAttribute CalculationPresets::Fast::genMPA() const {
        return CalMPAAttribute{8, 2, -4, CalMPASelectionMethod::HIGHEST, CalMPACompressionMethod::NO_COMPRESSION};
    }

    CalReferenceCompAttribute CalculationPresets::Fast::genReferenceCompression() const {
        return CalReferenceCompAttribute{1000000, 7, false};
    }

    std::string CalculationPresets::Normal::getName() const {
        return "Normal";
    }

    CalMPAAttribute CalculationPresets::Normal::genMPA() const {
        return CalMPAAttribute{8, 2, -5, CalMPASelectionMethod::HIGHEST, CalMPACompressionMethod::LITTLE_COMPRESSION};
    }

    CalReferenceCompAttribute CalculationPresets::Normal::genReferenceCompression() const {
        return CalReferenceCompAttribute{1000000, 11, false};
    }

    std::string CalculationPresets::Best::getName() const {
        return "Best";
    }

    CalMPAAttribute CalculationPresets::Best::genMPA() const {
        return CalMPAAttribute{8, 2, -6, CalMPASelectionMethod::HIGHEST, CalMPACompressionMethod::LITTLE_COMPRESSION};
    }

    CalReferenceCompAttribute CalculationPresets::Best::genReferenceCompression() const {
        return CalReferenceCompAttribute{1000000, 15, false};
    }

    std::string CalculationPresets::UltraBest::getName() const {
        return "Ultra Best";
    }

    CalMPAAttribute CalculationPresets::UltraBest::genMPA() const {
        return CalMPAAttribute{8, 2, -7, CalMPASelectionMethod::HIGHEST, CalMPACompressionMethod::LITTLE_COMPRESSION};
    }

    CalReferenceCompAttribute CalculationPresets::UltraBest::genReferenceCompression() const {
        return CalReferenceCompAttribute{1000000, 19, false};
    }

    std::string CalculationPresets::Stable::getName() const {
        return "Stable";
    }

    CalMPAAttribute CalculationPresets::Stable::genMPA() const {
        return CalMPAAttribute{8, 2, -4, CalMPASelectionMethod::HIGHEST, CalMPACompressionMethod::STRONGEST};
    }

    CalReferenceCompAttribute CalculationPresets::Stable::genReferenceCompression() const {
        return CalReferenceCompAttribute{1000000, 6, false};
    }

    std::string CalculationPresets::MoreStable::getName() const {
        return "More Stable";
    }

    CalMPAAttribute CalculationPresets::MoreStable::genMPA() const {
        return CalMPAAttribute{8, 2, -4, CalMPASelectionMethod::HIGHEST, CalMPACompressionMethod::STRONGEST};
    }

    CalReferenceCompAttribute CalculationPresets::MoreStable::genReferenceCompression() const {
        return CalReferenceCompAttribute{100000, 6, false};
    }

    std::string CalculationPresets::UltraStable::getName() const {
        return "Ultra Stable";
    }

    CalMPAAttribute CalculationPresets::UltraStable::genMPA() const {
        return CalMPAAttribute{8, 2, -4, CalMPASelectionMethod::HIGHEST, CalMPACompressionMethod::STRONGEST};
    }

    CalReferenceCompAttribute CalculationPresets::UltraStable::genReferenceCompression() const {
        return CalReferenceCompAttribute{10000, 6, true};
    }
}