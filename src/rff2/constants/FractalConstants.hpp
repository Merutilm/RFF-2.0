//
// Created by Merutilm on 2025-08-09.
//

#pragma once
#include <cstdint>
#include <memory>

namespace merutilm::rff2::Constants::Fractal {
    constexpr uint32_t HOTPATH_INTERRUPT_CHECK_INTERVAL = 1024;
    constexpr double ZOOM_MIN = 1.0;
    constexpr double ZOOM_INTERVAL = 0.235;
    constexpr double COMPUTESHADER_ZOOM_THRESHOLD = 35;
    constexpr double MULTITHREAD_ZOOM_THRESHOLD = 300;
    constexpr uint16_t GAUSSIAN_MAX_WIDTH = 200;
    constexpr uint32_t PARTITION_SIZE = 512;
    constexpr double MAX_LOC_LEN = 5;
    constexpr int64_t EXP10_ADDITION = 15;
    constexpr size_t MAX_PALETTE_LEN = 1000000;
} // namespace merutilm::rff2::Constants::Fractal
