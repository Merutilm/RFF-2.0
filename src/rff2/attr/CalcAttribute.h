#pragma once

#include "CalDecimalizeIterationMethod.h"
#include "CalMPAAttribute.h"
#include "CalReferenceCompAttribute.h"
#include "CalReuseReferenceMethod.h"
#include "../calc/fp_complex.h"


namespace merutilm::rff2 {
    struct CalcAttribute final{
        fp_complex center;
        float logZoom;
        uint64_t maxIteration;
        float bailout;
        CalDecimalizeIterationMethod decimalizeIterationMethod;
        CalMPAAttribute mpaAttribute;
        CalReferenceCompAttribute referenceCompAttribute;
        CalReuseReferenceMethod reuseReferenceMethod;
        bool autoMaxIteration;
        uint16_t autoIterationMultiplier;
        bool absoluteIterationMode;
    };
}
