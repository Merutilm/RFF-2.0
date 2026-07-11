//
// Created by Merutilm on 2026-05-13.
//

#pragma once
#include "../calc/fixed_point_complex.hpp"
#include "FrtReferenceCompSettings.h"
#include "FrtReferenceReuseMethod.h"
#include "FrtReferenceSyncSettings.hpp"

namespace merutilm::rff2 {
    struct FrtReferenceSettings {
        fixed_point_complex_i1 center;
        bool useParallelRefCalculation{};
        FrtReferenceSyncSettings sync{};
        FrtReferenceCompSettings compression{};
        FrtReferenceReuseMethod reuse{};
    };
}
