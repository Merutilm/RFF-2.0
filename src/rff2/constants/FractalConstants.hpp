//
// Created by Merutilm on 2025-08-09.
//

#pragma once
#include <chrono>

namespace merutilm::rff2::Constants::Fractal {
    constexpr int EXIT_CHECK_INTERVAL = 256;
    constexpr float ZOOM_MIN = 1.0f;
    constexpr float ZOOM_INTERVAL = 0.235f;
    constexpr float ZOOM_DEADLINE = 290;
    constexpr uint64_t MINIMUM_ITERATION = 100;
    constexpr uint16_t GAUSSIAN_MAX_WIDTH = 200;
    constexpr int GAUSSIAN_REQUIRES_BOX = 3;
    constexpr double INTENTIONAL_ERROR_OFFSET_MIN_PIX = 0.25;
    constexpr double INTENTIONAL_ERROR_DCLMB = 1e16; //DCmax for Locate Minibrot
    constexpr double INTENTIONAL_ERROR_REFZERO_POWER = 1024; // multiplier of exp10 when zr, zi is zero
    constexpr int EXP10_ADDITION = 15;
    inline static const unsigned long long INIT_TIME = std::chrono::system_clock::now().time_since_epoch().count();
}