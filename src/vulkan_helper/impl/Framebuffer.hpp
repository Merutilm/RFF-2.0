//
// Created by Merutilm on 2025-07-14.
//

#pragma once
#include "RenderPass.hpp"
#include "../handle/CoreHandler.hpp"

namespace merutilm::vkh {
    class Framebuffer final : public CoreHandler {
        std::vector<VkFramebuffer> framebuffer = {};
        const RenderPass &renderPass;
        VkExtent2D extent;

    public:
        explicit Framebuffer(const Core &core, const RenderPass &renderPass, VkExtent2D extent);

        ~Framebuffer() override;

        Framebuffer(const Framebuffer &) = delete;

        Framebuffer &operator=(const Framebuffer &) = delete;

        Framebuffer(Framebuffer &&) = delete;

        Framebuffer &operator=(Framebuffer &&) = delete;


        [[nodiscard]] VkFramebuffer getFramebufferHandle(const uint32_t imageIndex) const { return framebuffer[imageIndex]; }

        void setExtent(const VkExtent2D &extent) {
            this->extent = extent;
        }

    private:
        void init() override;

        void destroy() override;
    };
}
