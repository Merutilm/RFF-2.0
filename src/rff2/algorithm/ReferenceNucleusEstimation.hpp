//
// Created by Merutilm on 11/26/26.
//

#pragma once
#include "../calc/fixed_point_complex.hpp"
#include "ReferenceUtils.hpp"
namespace merutilm::rff2 {

    struct ReferenceNucleusEstimation {
        fixed_point_complex mbNucleus{};
        fixed_point_complex mbNucleusOff{};
        dex mbScale{};
        double mbRotation = 0;

        ReferenceNucleusEstimation() = default;

        explicit ReferenceNucleusEstimation(const fixed_point_complex &z, const fixed_point_complex &c,
                                       const complex<dex> &fzgAn, const complex<dex> &fpgBn, const int64_t exp10,
                                       std::array<fixed_point_decimal, fixed_point_complex::TEMPS_COUNT> &temps) {

            const auto mbScaleFactor = (fzgAn * fpgBn).try_normalized_value();
            mbScale = -mbScaleFactor.norm();
            mbRotation = rff_math::atan2(mbScaleFactor.im, mbScaleFactor.re);
            mbNucleusOff.set_exp10(exp10);
            mbNucleus.set_exp10(exp10);
            for (auto &temp : temps) {
                temp.set_exp10(exp10);
            }

            ReferenceUtils::calcCenterOffset(mbNucleusOff, z.create_variant(exp10), fixed_point_complex(fpgBn, exp10),
                                             temps);
            fixed_point_complex::add(mbNucleus, c, mbNucleusOff);
        }
    };
} // namespace merutilm::rff2
