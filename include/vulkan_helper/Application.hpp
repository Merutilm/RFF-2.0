//
// Created by Merutilm on 2026-02-06.
//

#pragma once
#include "engine/SharedResource.hpp"
#include "handle/EngineHandler.hpp"
namespace merutilm::vkh {


    struct Application : Handler {

        std::optional<Engine> engine;
        std::optional<SharedResource> sharedResource;

        explicit Application();

        ~Application() override;

        Application(const Application &) = delete;

        Application(Application &&) = delete;

        Application &operator=(const Application &) = delete;

        Application &operator=(Application &&) = delete;

    protected:
        void init() override;

        void cleanup() override;
    };


} // namespace merutilm::vkh
