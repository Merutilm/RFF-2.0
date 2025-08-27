//
// Created by Merutilm on 2025-07-13.
//

#pragma once

#include "Handler.hpp"
#include "../impl/Engine.hpp"

namespace merutilm::vkh {
    struct EngineHandler : Handler {
        EngineRef engine;

        explicit EngineHandler(EngineRef engine) : engine(engine) {};

        ~EngineHandler() override = default;

        EngineHandler(const EngineHandler &) = delete;

        EngineHandler &operator=(const EngineHandler &) = delete;

        EngineHandler(EngineHandler &&) = delete;

        EngineHandler &operator=(EngineHandler &&) = delete;

    };
}
