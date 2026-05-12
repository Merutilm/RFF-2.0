//
// Created by Merutilm on 2025-05-16.
//

#pragma once
#include "AppRenderManager.hpp"

namespace merutilm::rff2 {
    struct CallbackShader {
        static std::function<void()> fnPalette(AppRenderManager &arm);
        static std::function<void()> fnStripe(AppRenderManager &arm);
        static std::function<void()> fnSlope(AppRenderManager &arm);
        static std::function<void()> fnColor(AppRenderManager &arm);
        static std::function<void()> fnFog(AppRenderManager &arm);
        static std::function<void()> fnBloom(AppRenderManager &arm);
        static std::function<void()> fnLoadKfrPalette(AppRenderManager &arm);
    };
} // namespace merutilm::rff2
