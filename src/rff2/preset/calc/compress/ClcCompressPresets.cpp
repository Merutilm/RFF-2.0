//
// Created by Merutilm on 9/4/26.
//

#include "ClcCompressPresets.hpp"
namespace merutilm::rff2::ClcCompressPresets {

    std::string None::getName() const {
        return "None";
    }
    FrtMPASettings None::genMPA() const {
        return FrtMPASettings{4, 2, -4, FrtMPASelectionMethod::HIGHEST, false, true};
    }
    FrtReferenceCompSettings None::genRefComp() const {
        return FrtReferenceCompSettings{0, 0};
    }
    std::string Stable::getName() const { return "Stable"; }

    FrtMPASettings Stable::genMPA() const {
        return FrtMPASettings{4, 2, -4, FrtMPASelectionMethod::HIGHEST, true, true};
    }

    FrtReferenceCompSettings Stable::genRefComp() const { return FrtReferenceCompSettings{1000000, 6}; }

    std::string MoreStable::getName() const { return "More Stable"; }


    FrtMPASettings MoreStable::genMPA() const {
        return FrtMPASettings{4, 2, -4, FrtMPASelectionMethod::HIGHEST, true, true};
    }

    FrtReferenceCompSettings MoreStable::genRefComp() const { return FrtReferenceCompSettings{100000, 6}; }

    std::string UltraStable::getName() const { return "Ultra Stable"; }

    FrtMPASettings UltraStable::genMPA() const {
        return FrtMPASettings{8, 2, -4, FrtMPASelectionMethod::HIGHEST, true, true};
    }

    FrtReferenceCompSettings UltraStable::genRefComp() const { return FrtReferenceCompSettings{10000, 6}; }
} // namespace merutilm::rff2::ClcCompressPresets
