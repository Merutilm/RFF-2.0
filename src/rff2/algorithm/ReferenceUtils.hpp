//
// Created by Merutilm on 11/26/26.
//

#pragma once
#include "../calc/fixed_point_complex.hpp"

namespace merutilm::rff2::ReferenceUtils {

    inline void calcCenterOffset(fixed_point_complex &result, const fixed_point_complex &z,
                                 const fixed_point_complex &bn,
                                 std::array<fixed_point_decimal, fixed_point_complex::TEMPS_COUNT> &temps) {
        fixed_point_complex::div(result, z, bn, temps);
        result.neg();
    }

    template<Number Num>
    static complex<Num> calcCenterOffset(const complex<Num> &z, const complex<Num> &bn) {
        return -z / bn;
    }
} // namespace merutilm::rff2::ReferenceUtils
