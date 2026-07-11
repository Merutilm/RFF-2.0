//
// Created by Merutilm on 2025-05-31.
//

#include "CalculationPresets.h"


namespace merutilm::rff2 {
    std::string CalculationPresets::UltraFast::getName() const { return "Ultra Fast"; }

    FrtReferenceSyncSettings CalculationPresets::UltraFast::genRefSync() const {
        return FrtReferenceSyncSettings{32, 3};
    }


    FrtMPASettings CalculationPresets::UltraFast::genMPA() const {
        return FrtMPASettings{FrtMPADegree::P1_STANDARD,
                              4,
                              2,
                              -3,
                              FrtMPASelectionMethod::HIGHEST,
                              FrtMPACompressionMethod::NO_COMPRESSION};
    }

    FrtReferenceCompSettings CalculationPresets::UltraFast::genRefComp() const {
        return FrtReferenceCompSettings{0, 0, false};
    }

    std::string CalculationPresets::Fast::getName() const { return "Fast"; }


    FrtReferenceSyncSettings CalculationPresets::Fast::genRefSync() const { return FrtReferenceSyncSettings{16, 3}; }

    FrtMPASettings CalculationPresets::Fast::genMPA() const {
        return FrtMPASettings{FrtMPADegree::P1_STANDARD,
                              8,
                              2,
                              -4,
                              FrtMPASelectionMethod::HIGHEST,
                              FrtMPACompressionMethod::NO_COMPRESSION};
    }

    FrtReferenceCompSettings CalculationPresets::Fast::genRefComp() const {
        return FrtReferenceCompSettings{1000000, 7, false};
    }


    FrtReferenceSyncSettings CalculationPresets::Normal::genRefSync() const { return FrtReferenceSyncSettings{8, 2}; }

    std::string CalculationPresets::Normal::getName() const { return "Normal"; }

    FrtMPASettings CalculationPresets::Normal::genMPA() const {
        return FrtMPASettings{FrtMPADegree::P1_STANDARD,
                              8,
                              2,
                              -5,
                              FrtMPASelectionMethod::HIGHEST,
                              FrtMPACompressionMethod::LITTLE_COMPRESSION};
    }

    FrtReferenceCompSettings CalculationPresets::Normal::genRefComp() const {
        return FrtReferenceCompSettings{1000000, 11, false};
    }

    std::string CalculationPresets::Best::getName() const { return "Best"; }


    FrtReferenceSyncSettings CalculationPresets::Best::genRefSync() const { return FrtReferenceSyncSettings{4, 2}; }

    FrtMPASettings CalculationPresets::Best::genMPA() const {
        return FrtMPASettings{FrtMPADegree::P1_STANDARD,
                              8,
                              2,
                              -6,
                              FrtMPASelectionMethod::HIGHEST,
                              FrtMPACompressionMethod::LITTLE_COMPRESSION};
    }

    FrtReferenceCompSettings CalculationPresets::Best::genRefComp() const {
        return FrtReferenceCompSettings{1000000, 15, false};
    }

    std::string CalculationPresets::UltraBest::getName() const { return "Ultra Best"; }

    FrtReferenceSyncSettings CalculationPresets::UltraBest::genRefSync() const {
        return FrtReferenceSyncSettings{1, 1};
    }

    FrtMPASettings CalculationPresets::UltraBest::genMPA() const {
        return FrtMPASettings{FrtMPADegree::P1_STANDARD,
                              8,
                              2,
                              -7,
                              FrtMPASelectionMethod::HIGHEST,
                              FrtMPACompressionMethod::LITTLE_COMPRESSION};
    }

    FrtReferenceCompSettings CalculationPresets::UltraBest::genRefComp() const {
        return FrtReferenceCompSettings{1000000, 19, false};
    }

    std::string CalculationPresets::Stable::getName() const { return "Stable"; }


    FrtReferenceSyncSettings CalculationPresets::Stable::genRefSync() const { return FrtReferenceSyncSettings{16, 3}; }

    FrtMPASettings CalculationPresets::Stable::genMPA() const {
        return FrtMPASettings{FrtMPADegree::P1_STANDARD,         8, 2, -4, FrtMPASelectionMethod::HIGHEST,
                              FrtMPACompressionMethod::STRONGEST};
    }

    FrtReferenceCompSettings CalculationPresets::Stable::genRefComp() const {
        return FrtReferenceCompSettings{1000000, 6, false};
    }

    std::string CalculationPresets::MoreStable::getName() const { return "More Stable"; }

    FrtReferenceSyncSettings CalculationPresets::MoreStable::genRefSync() const {
        return FrtReferenceSyncSettings{16, 3};
    }

    FrtMPASettings CalculationPresets::MoreStable::genMPA() const {
        return FrtMPASettings{FrtMPADegree::P1_STANDARD,         8, 2, -4, FrtMPASelectionMethod::HIGHEST,
                              FrtMPACompressionMethod::STRONGEST};
    }

    FrtReferenceCompSettings CalculationPresets::MoreStable::genRefComp() const {
        return FrtReferenceCompSettings{100000, 6, false};
    }

    FrtReferenceSyncSettings CalculationPresets::UltraStable::genRefSync() const {
        return FrtReferenceSyncSettings{16, 3};
    }

    std::string CalculationPresets::UltraStable::getName() const { return "Ultra Stable"; }

    FrtMPASettings CalculationPresets::UltraStable::genMPA() const {
        return FrtMPASettings{FrtMPADegree::P1_STANDARD,         8, 2, -4, FrtMPASelectionMethod::HIGHEST,
                              FrtMPACompressionMethod::STRONGEST};
    }

    FrtReferenceCompSettings CalculationPresets::UltraStable::genRefComp() const {
        return FrtReferenceCompSettings{10000, 6, true};
    }
} // namespace merutilm::rff2
