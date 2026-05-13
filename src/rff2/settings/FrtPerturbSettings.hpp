//
// Created by Merutilm on 2026-05-13.
//

#pragma once
#include <cstdint>

#include "FrtDecimalizeIterationMethod.h"

namespace merutilm::rff2 {
    struct FrtPerturbSettings {
        uint64_t maxIteration;
        FrtDecimalizeIterationMethod decimalizeIterationMethod;
        bool autoMaxIteration;
        uint16_t autoIterationMultiplier;
        bool absoluteIterationMode;

    };
}