//
// Created by Merutilm on 2025-05-04.
//

#pragma once
#include <string>

#include "DecimalizeIterationMethod.h"
#include "MPACompressionMethod.h"
#include "MPASelectionMethod.h"
#include "ReuseReferenceMethod.h"

struct SelectableParser {

    static std::string parse(const ReuseReferenceMethod& value);
    static std::string parse(const DecimalizeIterationMethod& value);
    static std::string parse(const MPASelectionMethod& value);
    static std::string parse(const MPACompressionMethod& value);
};
