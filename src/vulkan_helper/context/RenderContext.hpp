//
// Created by Merutilm on 2025-07-18.
//

#pragma once
#include <memory>

#include "../configurator/RenderContextConfigurator.hpp"
#include "../impl/Framebuffer.hpp"
#include "../impl/RenderPass.hpp"

namespace merutilm::vkh {
    struct RenderContext final {
        const Core &core;
        std::unique_ptr<RenderContextConfigurator> configurator = nullptr;
        std::unique_ptr<RenderPass> renderPass = nullptr;
        std::unique_ptr<Framebuffer> framebuffer = nullptr;

        explicit RenderContext(const Core &core, const VkExtent2D &extent,
                               std::unique_ptr<RenderContextConfigurator> &&renderPassConfigurator) : core(core) {

            auto renderPassManager = Factory::create<RenderPassManager>();
            configurator = std::move(renderPassConfigurator);
            configurator->configure(renderPassManager);
            renderPass = std::make_unique<RenderPass>(core, std::move(renderPassManager));
            framebuffer = std::make_unique<Framebuffer>(core, *renderPass, extent);
        }

        void recreate(const VkExtent2D &extent) {
            framebuffer = nullptr;
            renderPass = nullptr;

            auto renderPassManager = Factory::create<RenderPassManager>();
            configurator->cleanupContexts();
            configurator->configure(renderPassManager);
            renderPass = std::make_unique<RenderPass>(core, std::move(renderPassManager));
            framebuffer = std::make_unique<Framebuffer>(core, *renderPass, extent);
        }
    };
}
