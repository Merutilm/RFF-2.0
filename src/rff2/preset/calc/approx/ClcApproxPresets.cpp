//
// Created by Merutilm on 2025-05-31.
//

#include "ClcApproxPresets.hpp"


namespace merutilm::rff2 {
    std::string ClcApproxPresets::UltraFast::getName() const { return "Ultra Fast"; }


    FrtMPASettings ClcApproxPresets::UltraFast::genMPA() const {
        return FrtMPASettings{4, 2, -3, FrtMPASelectionMethod::HIGHEST, false, true};
    }

    std::string ClcApproxPresets::Fast::getName() const { return "Fast"; }


    FrtMPASettings ClcApproxPresets::Fast::genMPA() const {
        return FrtMPASettings{8, 2, -4, FrtMPASelectionMethod::HIGHEST, false, true};
    }

    std::string ClcApproxPresets::Normal::getName() const { return "Normal"; }

    FrtMPASettings ClcApproxPresets::Normal::genMPA() const {
        return FrtMPASettings{8, 2, -5, FrtMPASelectionMethod::HIGHEST, false, true};
    }

    std::string ClcApproxPresets::Best::getName() const { return "Best"; }


    FrtMPASettings ClcApproxPresets::Best::genMPA() const {
        return FrtMPASettings{8, 2, -6, FrtMPASelectionMethod::HIGHEST, false, true};
    }

    std::string ClcApproxPresets::UltraBest::getName() const { return "Ultra Best"; }


    FrtMPASettings ClcApproxPresets::UltraBest::genMPA() const {
        return FrtMPASettings{8, 2, -7, FrtMPASelectionMethod::HIGHEST, false, true};
    }

    std::string ClcApproxPresets::LightSpirals::getName() const { return "Light Spirals"; }


    FrtMPASettings ClcApproxPresets::LightSpirals::genMPA() const {
        return FrtMPASettings{4, 2, -5, FrtMPASelectionMethod::HIGHEST, false, true};
    }

    std::string ClcApproxPresets::DenseSpirals::getName() const { return "Dense Spirals"; }


    FrtMPASettings ClcApproxPresets::DenseSpirals::genMPA() const {
        return FrtMPASettings{4, 2, -6, FrtMPASelectionMethod::HIGHEST, false, true};
    }
    std::string ClcApproxPresets::ExtremelyDenseSpirals::getName() const { return "Extremely Dense Spirals"; }

    FrtMPASettings ClcApproxPresets::ExtremelyDenseSpirals::genMPA() const {
        return FrtMPASettings{4, 2, -7, FrtMPASelectionMethod::HIGHEST, false, true};
    }
} // namespace merutilm::rff2
