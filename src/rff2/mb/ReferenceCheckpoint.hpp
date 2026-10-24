//
// Created by Merutilm on 9/12/26.
//

#pragma once
#include "../calc/fixed_point_complex.hpp"

namespace merutilm::rff2 {
    struct ReferenceCheckpoint {
        fixed_point_complex z;
        itt::ref refIteration = 0;
        complex<dex> fzgAn;
    };
}