//
// Created by Merutilm on 2025-07-18.
//

#pragma once
#include <memory>

#include "../configurator/RenderContextConfigurator.hpp"
#include "../impl/Framebuffer.hpp"
#include "../impl/RenderPass.hpp"

namespace merutilm::vkh {
    class RenderContextImpl final {
        CoreRef core;
        RenderContextConfigurator configurator = nullptr;
        RenderPass renderPass = nullptr;
        Framebuffer framebuffer = nullptr;

    public:
        explicit RenderContextImpl(CoreRef core, const VkExtent2D &extent,
                               RenderContextConfigurator &&renderPassConfigurator) : core(core) {

            auto renderPassManager = Factory::create<RenderPassManager>();
            configurator = std::move(renderPassConfigurator);
            configurator->configure(extent, *renderPassManager);
            renderPass = Factory::create<RenderPass>(core, std::move(renderPassManager));
            framebuffer = Factory::create<Framebuffer>(core, *renderPass, extent);
        }

        void recreate(const VkExtent2D &extent) {
            framebuffer = nullptr;
            renderPass = nullptr;

            auto renderPassManager = Factory::create<RenderPassManager>();
            configurator->cleanupContexts();
            configurator->configure(extent, *renderPassManager);
            renderPass = Factory::create<RenderPass>(core, std::move(renderPassManager));
            framebuffer = Factory::create<Framebuffer>(core, *renderPass, extent);
        }

        RenderContextConfiguratorPtr getConfigurator() const {
            return configurator.get();
        }

        RenderPassPtr getRenderPass() const {
            return renderPass.get();
        }

        FramebufferPtr getFramebuffer() const {
            return framebuffer.get();
        }

    };

    using RenderContext = std::unique_ptr<RenderContextImpl>;
    using RenderContextPtr = RenderContextImpl *;
    using RenderContextRef = RenderContextImpl &;
}
