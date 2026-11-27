//
// Created by Merutilm on 11/26/26.
//

#pragma once
#include "../calc/fixed_point_complex.hpp"
#include "ReferenceUtils.hpp"
namespace merutilm::rff2 {

    // Hierarchical Nested Reference Estimation Info.
    struct HNREInfo {
        fixed_point_complex mbNucleus{};
        fixed_point_complex mbNucleusOff{};
        fixed_point_complex transformed{};

        HNREInfo() = default;

        explicit HNREInfo(fixed_point_complex &z, const fixed_point_complex &c,
                                            const fixed_point_complex &fzgAn, const complex<dex> &fpgBn,
                                            const int64_t exp10,
                                            std::array<fixed_point_decimal, fixed_point_util::TEMPS_COUNT> &temps) {

            mbNucleusOff.set_exp10(exp10);
            mbNucleus.set_exp10(exp10);
            transformed.set_exp10(exp10);

            for (auto &temp: temps) {
                temp.set_exp10(exp10, false);
            }

            z.set_exp10(exp10);
            ReferenceUtils::calcCenterOffset(mbNucleusOff, z, fixed_point_complex(fpgBn.re, fpgBn.im, exp10),temps);

            fixed_point_complex::mul(transformed, fzgAn, z, temps);

            // std::cout << "[DEBUG]: " << transformed << std::endl;

            fixed_point_complex::add(mbNucleus, c, mbNucleusOff);
        }

    };
} // namespace merutilm::rff2
