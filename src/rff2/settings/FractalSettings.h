#pragma once

#include "../calc/fp_complex.h"
#include "FrtDecimalizeIterationMethod.h"
#include "FrtMPASettings.h"
#include "FrtReferenceCompSettings.h"
#include "FrtReferenceSyncSettings.hpp"
#include "FrtReuseReferenceMethod.h"


namespace merutilm::rff2 {
    struct FractalSettings final{
        fp_complex center;
        float logZoom;
        uint64_t maxIteration;
        float bailout;
        FrtReferenceSyncSettings referenceSyncSettings;
        FrtDecimalizeIterationMethod decimalizeIterationMethod;
        FrtMPASettings mpaSettings;
        FrtReferenceCompSettings referenceCompSettings;
        FrtReuseReferenceMethod reuseReferenceMethod;
        bool autoMaxIteration;
        uint16_t autoIterationMultiplier;
        bool absoluteIterationMode;
    };
}
