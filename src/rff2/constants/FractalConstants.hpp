//
// Created by Merutilm on 2025-08-09.
//

#pragma once
#include <cstdint>
#include <memory>

namespace merutilm::rff2::Constants::Fractal {
    constexpr int PARALLEL_OPERATION_INTERRUPT_CHECK_INTERVAL = 1024;
    constexpr float ZOOM_MIN = 1.0f;
    constexpr float ZOOM_INTERVAL = 0.235f;
    constexpr float NM_ZOOM_DEADLINE = 35;
    constexpr float DB_ZOOM_DEADLINE = 300;
    constexpr uint16_t GAUSSIAN_MAX_WIDTH = 200;
    constexpr double MAX_LOC_LEN = 5;
    constexpr int EXP10_ADDITION = 15;
    constexpr size_t MAX_PALETTE_LEN = 1000000;
} // namespace merutilm::rff2::Constants::Fractal
