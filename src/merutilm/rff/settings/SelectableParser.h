//
// Created by Merutilm on 2025-05-04.
//

#pragma once
#include <string>

#include "ColorSmoothingSettings.h"
#include "DecimalizeIterationMethod.h"
#include "MPACompressionMethod.h"
#include "MPASelectionMethod.h"
#include "ReuseReferenceMethod.h"
#include "StripeType.h"

struct SelectableParser {

    static std::string parse(const ReuseReferenceMethod& value);
    static std::string parse(const DecimalizeIterationMethod& value);
    static std::string parse(const MPASelectionMethod& value);
    static std::string parse(const MPACompressionMethod& value);
    static std::string parse(const ColorSmoothingSettings& value);
    static std::string parse(const StripeType &value);
};
