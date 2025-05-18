#pragma once

#include "DecimalizeIterationMethod.h"
#include "MPASettings.h"
#include "ReferenceCompressionSettings.h"
#include "ReuseReferenceMethod.h"
#include "../calc/fp_complex.h"


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
    bool absoluteIterationMode;
};

