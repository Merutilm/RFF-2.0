//
// Created by Merutilm on 2025-07-13.
//

#pragma once

#include <vulkan_helper/engine/Engine.hpp>
#include <vulkan_helper/handle/Handler.hpp>

namespace merutilm::vkh {
    struct EngineHandler : Handler {
        Engine & engine;

        explicit EngineHandler(Engine & engine) : engine(engine) {};

        ~EngineHandler() override = default;

        EngineHandler(EngineHandler &) = delete;

        EngineHandler &operator=(EngineHandler &) = delete;

        EngineHandler(EngineHandler &&) = default;

        EngineHandler &operator=(EngineHandler &&) = delete;

    };
}
