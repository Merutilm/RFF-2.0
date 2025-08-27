//
// Created by Merutilm on 2025-07-11.
//

#pragma once
#include <memory>

#include "../context/ImageContext.hpp"
#include "../manage/RenderPassManager.hpp"
#include "../handle/CoreHandler.hpp"

namespace merutilm::vkh {
    class RenderPassImpl final : public CoreHandler {

        RenderPassManager manager = nullptr;
        VkRenderPass renderPass = VK_NULL_HANDLE;

    public:
        explicit RenderPassImpl(const CoreRef core, RenderPassManager &&manager);

        ~RenderPassImpl() override;

        RenderPassImpl(const RenderPassImpl &) = delete;

        RenderPassImpl &operator=(const RenderPassImpl &) = delete;

        RenderPassImpl(RenderPassImpl &&) = delete;

        RenderPassImpl &operator=(RenderPassImpl &&) = delete;

        [[nodiscard]] RenderPassManagerRef getManager() const { return *manager; }

        [[nodiscard]] VkRenderPass getRenderPassHandle() const { return renderPass; }

    private:
        void init() override;

        void destroy() override;
    };

    using RenderPass = std::unique_ptr<RenderPassImpl>;
    using RenderPassPtr = RenderPassImpl *;
    using RenderPassRef = RenderPassImpl &;
}
