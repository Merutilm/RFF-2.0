//
// Created by Merutilm on 2026-05-13.
//

#pragma once
#include <cstdint>
#include "../calc/iteration_types.hpp"

namespace merutilm::rff2 {
    struct FrtPerturbSettings {
        itt::ptb maxIteration;
        bool smoothIteration;
        bool autoMaxIteration;
        uint8_t interiorDetectRadiusPower;
        uint16_t autoIterationMultiplier;
        bool absoluteIterationMode;

    };
}