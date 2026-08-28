//
// Created by Merutilm on 7/26/26.
//

#pragma once

namespace merutilm::rff2::Constants::Render {
    constexpr uint32_t MIN_WINDOW_WIDTH = 100;
    constexpr uint32_t MIN_WINDOW_HEIGHT = 100;

    constexpr int INIT_WINDOW_WIDTH = 1280;
    constexpr int INIT_WINDOW_HEIGHT = 720;

    constexpr float MIN_CLARITY_MULTIPLIER = 0.125;
    constexpr float MAX_CLARITY_MULTIPLIER = 4;

    constexpr float MIN_FPS = 30;
    constexpr float INIT_FPS = 100;
    constexpr float MAX_FPS = 240;

    constexpr uint32_t COMPUTE_SHADER_INIT_BATCH_SIZE = 128;
}