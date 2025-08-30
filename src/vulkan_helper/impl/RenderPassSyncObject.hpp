//
// Created by Merutilm on 2025-08-29.
//

#pragma once
#include <vector>

#include "Core.hpp"
#include "../handle/CoreHandler.hpp"

namespace merutilm::vkh {
    class RenderPassSyncObjectImpl final : public CoreHandler {
        std::vector<VkFence> fences;

    public:
        explicit RenderPassSyncObjectImpl(CoreRef core);

        ~RenderPassSyncObjectImpl() override;

        RenderPassSyncObjectImpl(const RenderPassSyncObjectImpl &) = delete;

        RenderPassSyncObjectImpl &operator=(const RenderPassSyncObjectImpl &) = delete;

        RenderPassSyncObjectImpl(RenderPassSyncObjectImpl &&) = delete;

        RenderPassSyncObjectImpl &operator=(RenderPassSyncObjectImpl &&) = delete;

        [[nodiscard]] VkFence getFence(const uint32_t frameIndex) const { return fences[frameIndex]; }

    private:
        void init() override;

        void destroy() override;
    };

    using RenderPassSyncObject = std::unique_ptr<RenderPassSyncObjectImpl>;
    using RenderPassSyncObjectPtr = RenderPassSyncObjectImpl *;
    using RenderPassSyncObjectRef = RenderPassSyncObjectImpl &;
};
