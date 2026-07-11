//
// Created by Merutilm on 2025-08-29.
//

#pragma once

namespace merutilm::vkh::config {
#ifdef NDEBUG
    static constexpr bool ENABLE_VALIDATION = false;
#else
    static constexpr bool ENABLE_VALIDATION = true;
#endif
    static constexpr float INITIAL_FPS = 60.0f;
}
