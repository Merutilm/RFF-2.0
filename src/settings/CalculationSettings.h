#pragma once

#include "DecimalizeIterationMethod.h"
#include "MPASettings.h"
#include "ReferenceCompressionSettings.h"
#include "ReuseReferenceMethod.h"
#include "../calc/fp_complex.h"


namespace merutilm::rff {
    struct CalculationSettings final{
        fp_complex center;
        float logZoom;
        uint64_t maxIteration;
        float bailout;
        DecimalizeIterationMethod decimalizeIterationMethod;
        MPASettings mpaSettings;
        ReferenceCompressionSettings referenceCompressionSettings;
        ReuseReferenceMethod reuseReferenceMethod;
        bool autoMaxIteration;
        uint16_t autoIterationMultiplier;
        bool absoluteIterationMode;
    };
}
