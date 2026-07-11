//
// Created by Merutilm on 2025-05-04.
//

#pragma once
#include <cstdint>

#include "FrtMPACompressionMethod.h"
#include "FrtMPADegree.hpp"
#include "FrtMPASelectionMethod.h"


namespace merutilm::rff2 {
    struct FrtMPASettings final {
        FrtMPADegree maxDegree;
        uint16_t minSkipReference;
        uint8_t maxMultiplierBetweenLevel;
        float epsilonPower;
        FrtMPASelectionMethod mpaSelectionMethod;
        FrtMPACompressionMethod mpaCompressionMethod;
    };
}