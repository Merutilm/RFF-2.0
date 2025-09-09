#pragma once

#include "FrtDecimalizeIterationMethod.h"
#include "FrtMPAAttribute.h"
#include "FrtReferenceCompAttribute.h"
#include "FrtReuseReferenceMethod.h"
#include "../calc/fp_complex.h"


namespace merutilm::rff2 {
    struct FractalAttribute final{
        fp_complex center;
        float logZoom;
        uint64_t maxIteration;
        float bailout;
        FrtDecimalizeIterationMethod decimalizeIterationMethod;
        FrtMPAAttribute mpaAttribute;
        FrtReferenceCompAttribute referenceCompAttribute;
        FrtReuseReferenceMethod reuseReferenceMethod;
        bool autoMaxIteration;
        uint16_t autoIterationMultiplier;
        bool absoluteIterationMode;
    };
}
