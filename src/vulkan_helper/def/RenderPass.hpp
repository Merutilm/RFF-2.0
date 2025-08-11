//
// Created by Merutilm on 2025-07-11.
//

#pragma once
#include <memory>

#include "../context/ImageContext.hpp"
#include "../manage/RenderPassManager.hpp"
#include "../handle/CoreHandler.hpp"

namespace merutilm::vkh {
    class RenderPass final : public CoreHandler {

        std::unique_ptr<RenderPassManager> manager = nullptr;
        VkRenderPass renderPass = VK_NULL_HANDLE;

    public:
        explicit RenderPass(const Core &core, std::unique_ptr<RenderPassManager> &&manager);

        ~RenderPass() override;

        RenderPass(const RenderPass &) = delete;

        RenderPass &operator=(const RenderPass &) = delete;

        RenderPass(RenderPass &&) = delete;

        RenderPass &operator=(RenderPass &&) = delete;

        [[nodiscard]] const RenderPassManager &getManager() const { return *manager; }

        [[nodiscard]] VkRenderPass getRenderPassHandle() const { return renderPass; }

    private:
        void init() override;

        void destroy() override;
    };
}
