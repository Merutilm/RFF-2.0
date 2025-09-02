//
// Created by Merutilm on 2025-07-18.
//

#pragma once

#include "../configurator/RenderContextConfigurator.hpp"
#include "../impl/Framebuffer.hpp"
#include "../impl/RenderPass.hpp"

namespace merutilm::vkh {
    class RenderContextImpl final {
        CoreRef core;
        RenderContextConfigurator renderContextConfigurator = nullptr;
        RenderPass renderPass = nullptr;
        Framebuffer framebuffer = nullptr;
        std::function<VkExtent2D()> extentGetter;

    public:
        explicit RenderContextImpl(CoreRef core, std::function<VkExtent2D()> &&extentGetter,
                                   RenderContextConfigurator &&renderPassConfigurator) : core(core),
            extentGetter(std::move(extentGetter)) {
            auto renderPassManager = factory::create<RenderPassManager>();
            renderContextConfigurator = std::move(renderPassConfigurator);
            const VkExtent2D extent = this->extentGetter();
            renderContextConfigurator->configure(*renderPassManager);
            renderPass = factory::create<RenderPass>(core, std::move(renderPassManager));
            framebuffer = factory::create<Framebuffer>(core, *renderPass, extent);
        }

        ~RenderContextImpl() = default;

        RenderContextImpl(const RenderContextImpl &) = delete;

        RenderContextImpl &operator=(const RenderContextImpl &) = delete;

        RenderContextImpl(RenderContextImpl &&) = delete;

        RenderContextImpl &operator=(RenderContextImpl &&) = delete;

        void recreate() {
            framebuffer = nullptr;
            renderPass = nullptr;
            VkExtent2D extent = extentGetter();
            auto renderPassManager = factory::create<RenderPassManager>();
            renderContextConfigurator->configure(*renderPassManager);
            renderPass = factory::create<RenderPass>(core, std::move(renderPassManager));
            framebuffer = factory::create<Framebuffer>(core, *renderPass, extent);
        }

        [[nodiscard]] RenderContextConfiguratorPtr getConfigurator() const {
            return renderContextConfigurator.get();
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
