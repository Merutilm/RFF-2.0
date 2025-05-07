//
// Created by Merutilm on 2025-05-04.
//

#pragma once
#include <cstdint>

#include "MPACompressionMethod.h"
#include "MPASelectionMethod.h"

struct MPASettings final {
    uint16_t minSkipReference;
    uint8_t maxMultiplierBetweenLevel;
    float epsilonPower;
    MPASelectionMethod mpaSelectionMethod;
    MPACompressionMethod mpaCompressionMethod;
};
