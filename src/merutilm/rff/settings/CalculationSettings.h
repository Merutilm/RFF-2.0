#pragma once

#include "DecimalizeIterationMethod.h"
#include "MPASettings.h"
#include "ReferenceCompressionSettings.h"
#include "ReuseReferenceMethod.h"
#include "../precision/Center.h"


struct CalculationSettings final{
    Center center;
    float logZoom;
    uint64_t maxIteration;
    float bailout;
    DecimalizeIterationMethod decimalizeIterationMethod;
    MPASettings mpaSettings;
    ReferenceCompressionSettings referenceCompressionSettings;
    ReuseReferenceMethod reuseReferenceMethod;
    bool autoIteration;
    bool absoluteIterationMode;
};

