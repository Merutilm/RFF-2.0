//
// Created by Merutilm on 2025-05-14.
//

#pragma once
#include <functional>

#include "AppRenderManager.hpp"
namespace merutilm::rff2 {
    struct CallbackFractal {
        static std::function<void()> fnReference(AppRenderManager &arm);
        static std::function<void()> fnIterations(AppRenderManager &arm);
        static std::function<void()> fnSa(AppRenderManager &arm);
        static std::function<void()> fnMpa(AppRenderManager &arm);

        static std::function<bool()> fnGetterAutomaticIterations(AppRenderManager &arm);
        static std::function<bool()> fnGetterAbsoluteIterationMode(AppRenderManager &arm);

        static std::function<void(bool)> fnAutomaticIterations(AppRenderManager &arm);
        static std::function<void(bool)> fnAbsoluteIterationMode(AppRenderManager &arm);
    };
} // namespace merutilm::rff2
