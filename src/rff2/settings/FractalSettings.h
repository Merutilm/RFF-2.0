#pragma once

#include "../calc/fixed_point_complex.hpp"
#include "FrtDecimalizeIterationMethod.h"
#include "FrtMPASettings.h"
#include "FrtReferenceCompSettings.h"
#include "FrtReferenceSyncSettings.hpp"
#include "FrtReuseReferenceMethod.h"


namespace merutilm::rff2 {
    struct FractalSettings final{
        fixed_point_complex_i1 center;
        float logZoom;
        uint64_t maxIteration;
        float bailout;
        FrtReferenceSyncSettings referenceSyncSettings;
        FrtDecimalizeIterationMethod decimalizeIterationMethod;
        FrtMPASettings mpaSettings;
        FrtReferenceCompSettings referenceCompSettings;
        FrtReuseReferenceMethod reuseReferenceMethod;
        bool useParallelRefCalculation;
        bool autoMaxIteration;
        uint16_t autoIterationMultiplier;
        bool absoluteIterationMode;
    };
}
