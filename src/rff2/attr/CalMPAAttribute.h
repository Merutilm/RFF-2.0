//
// Created by Merutilm on 2025-05-04.
//

#pragma once
#include <cstdint>

#include "CalMPACompressionMethod.h"
#include "CalMPASelectionMethod.h"


namespace merutilm::rff2 {
    struct CalMPAAttribute final {
        uint16_t minSkipReference;
        uint8_t maxMultiplierBetweenLevel;
        float epsilonPower;
        CalMPASelectionMethod mpaSelectionMethod;
        CalMPACompressionMethod mpaCompressionMethod;
    };
}