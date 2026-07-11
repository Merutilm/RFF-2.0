//
// Created by Merutilm on 2025-07-18.
//

#pragma once

#include <vulkan_helper/engine/graphics/Framebuffer.hpp>
#include <vulkan_helper/engine/graphics/RenderPass.hpp>

#include "vulkan_helper/engine/graphics/RenderPassGraphGeneratorBase.hpp"

namespace merutilm::vkh {

    class RenderContext final {
        Core &core;
        std::unique_ptr<RenderPassGraphGeneratorBase> renderPassGraphGenerator;
        std::optional<RenderPass> renderPass = std::nullopt;
        std::optional<Framebuffer> framebuffer = std::nullopt;
        std::function<VkExtent2D()> extentGetter;

    public:
        explicit RenderContext(Core &core, std::function<VkExtent2D()> &&extentGetter, std::unique_ptr<RenderPassGraphGeneratorBase> &&renderPassGraphGenerator);

        ~RenderContext() = default;

        RenderContext(const RenderContext &) = delete;

        RenderContext &operator=(const RenderContext &) = delete;

        RenderContext(RenderContext &&) = delete;

        RenderContext &operator=(RenderContext &&) = delete;

        void recreate();

        [[nodiscard]] RenderPassGraphGeneratorBase *getGenerator() const { return renderPassGraphGenerator.get(); }

        [[nodiscard]] RenderPass *getRenderPass() { return &*renderPass; }

        [[nodiscard]] Framebuffer *getFramebuffer() { return &*framebuffer; }

    };


} // namespace merutilm::vkh
