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
        std::function<VkExtent2D()> extentGetter;

    public:
        explicit RenderContextImpl(CoreRef core, std::function<VkExtent2D()> &&extentGetter,
                               RenderContextConfigurator &&renderPassConfigurator) : core(core), extentGetter(std::move(extentGetter)) {

            auto renderPassManager = Factory::create<RenderPassManager>();
            configurator = std::move(renderPassConfigurator);
            const VkExtent2D extent = this->extentGetter();
            configurator->configure(extent, *renderPassManager);
            renderPass = Factory::create<RenderPass>(core, std::move(renderPassManager));
            framebuffer = Factory::create<Framebuffer>(core, *renderPass, extent);
        }

        void recreate() {
            framebuffer = nullptr;
            renderPass = nullptr;
            VkExtent2D extent = extentGetter();
            auto renderPassManager = Factory::create<RenderPassManager>();
            configurator->cleanupContexts();
            configurator->configure(extent, *renderPassManager);
            renderPass = Factory::create<RenderPass>(core, std::move(renderPassManager));
            framebuffer = Factory::create<Framebuffer>(core, *renderPass, extent);
        }

        [[nodiscard]] RenderContextConfiguratorPtr getConfigurator() const {
            return configurator.get();
        }

        [[nodiscard]] RenderPassPtr getRenderPass() const {
            return renderPass.get();
        }

        [[nodiscard]] FramebufferPtr getFramebuffer() const {
            return framebuffer.get();
        }

    };

    using RenderContext = std::unique_ptr<RenderContextImpl>;
    using RenderContextPtr = RenderContextImpl *;
    using RenderContextRef = RenderContextImpl &;
}
