//
// Created by Merutilm on 2026-05-13.
//

#pragma once
#include <cstdint>
namespace merutilm::rff2 {
    struct FrtGeneralSettings {
        float bailout;
        double logZoom;
        uint32_t threads;
    };
}