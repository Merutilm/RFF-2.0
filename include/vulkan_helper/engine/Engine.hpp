//
// Created by Merutilm on 2025-07-19.
//

#pragma once
#include <vulkan_helper/base/pch.hpp>

#include <vulkan_helper/engine/context/WindowContext.hpp>
#include <vulkan_helper/core/Core.hpp>
#include "SharedResource.hpp"

namespace merutilm::vkh {
    class Engine final : public Handler {
        Core core;
        Repositories globalRepositories;
        std::unique_ptr<SharedResource> sharedResource;
        std::vector<std::unique_ptr<WindowContext>> windowContexts = {};

    public:
        explicit Engine();

        ~Engine() override;

        Engine(const Engine &) = delete;

        Engine &operator=(const Engine &) = delete;

        Engine(Engine &&) = delete;

        Engine &operator=(Engine &&) = delete;

        [[nodiscard]] bool isValidWindowContext(uint32_t windowAttachmentIndex) const;

        [[nodiscard]] WindowContext & attachWindowContext(WindowInitializerSettings &&wic, uint32_t windowAttachmentIndexExpected);

        std::unique_ptr<WindowContext> detachWindowContext(uint32_t windowAttachmentIndex);


        [[nodiscard]] Core & getCore() { return core; }

        [[nodiscard]] WindowContext & getWindowContext(const uint32_t windowContextIndex) const {
            return *windowContexts.at(windowContextIndex);
        }

        [[nodiscard]] Repositories & getGlobalRepositories()  {
            return globalRepositories;
        }

        [[nodiscard]] SharedResource & getSharedResource() const {
            return *sharedResource;
        }

        [[nodiscard]] uint32_t getWindowContextCount() const { return windowContexts.size(); }

    protected:
        void init() override;

        void cleanup() override;

    private:
        void configureRepositories();
    };



}
