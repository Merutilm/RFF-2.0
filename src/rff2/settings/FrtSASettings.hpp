//
// Created by Merutilm on 2026-06-01.
//

#pragma once
#include <cstdint>
namespace merutilm::rff2 {
    struct FrtSASettings {
        uint16_t appliedTermsCount;
        uint16_t validatedTermsCount;
        float epsilonPower;
    };
}
