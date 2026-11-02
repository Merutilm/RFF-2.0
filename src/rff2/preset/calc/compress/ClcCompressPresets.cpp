//
// Created by Merutilm on 9/4/26.
//

#include "ClcCompressPresets.hpp"
namespace merutilm::rff2::ClcCompressPresets {

    std::string None::getName() const {
        return "None";
    }
    FrtReferenceCompSettings None::genRefComp() const {
        return FrtReferenceCompSettings{0, 0};
    }
    std::string SemiCompressed::getName() const { return "Semi Compressed"; }

    FrtReferenceCompSettings SemiCompressed::genRefComp() const { return FrtReferenceCompSettings{1000000, 6}; }

    std::string Compressed::getName() const { return "Compressed"; }


    FrtReferenceCompSettings Compressed::genRefComp() const { return FrtReferenceCompSettings{100000, 6}; }

    std::string MoreCompressed::getName() const { return "More Compressed"; }

    FrtReferenceCompSettings MoreCompressed::genRefComp() const { return FrtReferenceCompSettings{10000, 6}; }
} // namespace merutilm::rff2::ClcCompressPresets
