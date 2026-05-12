//
// Created by Merutilm on 2025-07-14.
//

#pragma once
#include "RenderPass.hpp"
#include <vulkan_helper/handle/CoreHandler.hpp>

namespace merutilm::vkh {
    class Framebuffer final : public CoreHandler {
        std::vector<VkFramebuffer> framebuffer = {};
        RenderPass & renderPass;
        VkExtent2D extent;

    public:
        explicit Framebuffer(Core & core, RenderPass & renderPass, VkExtent2D extent);

        ~Framebuffer() override;

        Framebuffer(const Framebuffer &) = delete;

        Framebuffer &operator=(const Framebuffer &) = delete;

        Framebuffer(Framebuffer &&) = delete;

        Framebuffer &operator=(Framebuffer &&) = delete;


        [[nodiscard]] VkFramebuffer getFramebufferHandle(const uint32_t imageIndex) const { return framebuffer[imageIndex]; }

        [[nodiscard]] const VkExtent2D &getExtent() const { return extent; }

    private:
        void init() override;

        void cleanup() override;
    };

}
