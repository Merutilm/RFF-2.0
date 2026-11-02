//
// Created by Merutilm on 2025-05-31.
//

#include "ClcApproxPresets.hpp"


namespace merutilm::rff2 {
    std::string ClcApproxPresets::UltraFast::getName() const { return "Ultra Fast"; }


    FrtMPASettings ClcApproxPresets::UltraFast::genMPA() const {
        return FrtMPASettings{.minSkipReference = 4,
                              .maxMultiplierBetweenLevel = 2,
                              .epsilonPower = -3.15f,
                              .selectionMethod = FrtMPASelectionMethod::HIGHEST};
    }

    std::string ClcApproxPresets::Fast::getName() const { return "Fast"; }


    FrtMPASettings ClcApproxPresets::Fast::genMPA() const {
        return FrtMPASettings{.minSkipReference = 4,
                              .maxMultiplierBetweenLevel = 2,
                              .epsilonPower = -4,
                              .selectionMethod = FrtMPASelectionMethod::HIGHEST};
    }

    std::string ClcApproxPresets::Normal::getName() const { return "Normal"; }

    FrtMPASettings ClcApproxPresets::Normal::genMPA() const {
        return FrtMPASettings{.minSkipReference = 4,
                              .maxMultiplierBetweenLevel = 2,
                              .epsilonPower = -5,
                              .selectionMethod = FrtMPASelectionMethod::HIGHEST};
    }

    std::string ClcApproxPresets::Best::getName() const { return "Best"; }


    FrtMPASettings ClcApproxPresets::Best::genMPA() const {
        return FrtMPASettings{.minSkipReference = 4,
                              .maxMultiplierBetweenLevel = 2,
                              .epsilonPower = -6,
                              .selectionMethod = FrtMPASelectionMethod::HIGHEST};
    }

    std::string ClcApproxPresets::UltraBest::getName() const { return "Ultra Best"; }


    FrtMPASettings ClcApproxPresets::UltraBest::genMPA() const {
        return FrtMPASettings{.minSkipReference = 4,
                              .maxMultiplierBetweenLevel = 2,
                              .epsilonPower = -7,
                              .selectionMethod = FrtMPASelectionMethod::HIGHEST};
    }
} // namespace merutilm::rff2
