//
// Created by Merutilm on 11/27/26.
//

#pragma once
#include <array>
#include <cstdint>
namespace merutilm::rff2::fixed_point_util {
    static constexpr uint64_t TEMPS_COUNT = 6;

    inline std::array<fixed_point_decimal, TEMPS_COUNT> create_temps(const int64_t exp10) {
        std::array<fixed_point_decimal, TEMPS_COUNT> result;
        for (fixed_point_decimal &temp : result) {
            temp.set_exp10(exp10);
        }
        return result;
    }
}