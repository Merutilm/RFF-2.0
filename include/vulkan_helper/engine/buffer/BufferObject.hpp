//
// Created by Merutilm on 2025-07-10.
//

#pragma once
#include <vulkan_helper/engine/buffer/HostDataObject.hpp>
#include <vulkan_helper/engine/cmd/CommandPool.hpp>
#include <vulkan_helper/engine/context/BufferContext.hpp>
#include <vulkan_helper/engine/manage/HostDataObjectManager.hpp>
#include <vulkan_helper/engine/sync/Fence.hpp>
#include <vulkan_helper/engine/wrapped/BufferLocalization.hpp>
#include <vulkan_helper/handle/CoreHandler.hpp>

namespace merutilm::vkh {
    class BufferObject : public CoreHandler {
        HostDataObject hostDataObject;
        VkBufferUsageFlags bufferUsage;
        std::variant<BufferContext, MultiframeBufferContext> bufferContext = {};
        BufferLocalization bufferLocalization;
        bool localized = false;
        const bool multiframeEnabled;

    public:
        explicit BufferObject(Core &core, HostDataObjectManager &&dataManager, VkBufferUsageFlags bufferUsage,
                               BufferLocalization bufferLocalization, bool multiframeEnabled);

        ~BufferObject() override;

        void reloadBuffer();

        BufferObject(const BufferObject &) = delete;

        BufferObject &operator=(const BufferObject &) = delete;

        BufferObject(BufferObject &&) = delete;

        BufferObject &operator=(BufferObject &&) noexcept = delete;

        void localize(CommandPool & commandPool, const Fence * fence = VK_NULL_HANDLE);

        void expose(CommandPool & commandPool, const Fence * fence = VK_NULL_HANDLE);

        [[nodiscard]] MultiframeBufferContext &getBufferContextMF() {
            if (multiframeEnabled) return std::get<MultiframeBufferContext>(bufferContext);
            throw exception_invalid_state("current object is not multiframed");
        }

        [[nodiscard]] const MultiframeBufferContext &getBufferContextMF() const {
            if (multiframeEnabled) return std::get<MultiframeBufferContext>(bufferContext);
            throw exception_invalid_state("current object is not multiframed (const)");
        }

        [[nodiscard]] const BufferContext &getBufferContextMF(const uint32_t frameIndex) const {
            if (multiframeEnabled) return std::get<MultiframeBufferContext>(bufferContext)[frameIndex];
            throw exception_invalid_state("current object is not multiframed");
        }
        [[nodiscard]] BufferContext &getBufferContext() {
            if (!multiframeEnabled) return std::get<BufferContext>(bufferContext);
            throw exception_invalid_state("current object is multiframed");
        }

        [[nodiscard]] const BufferContext &getBufferContext() const {
            if (!multiframeEnabled) return std::get<BufferContext>(bufferContext);
            throw exception_invalid_state("current object is multiframed (const)");
        }

        void update() const;

        void update(uint32_t target) const;

        void updateMF(uint32_t frameIndex) const;

        void updateMF(uint32_t frameIndex, uint32_t target) const;

        void checkFinalizedBeforeUpdate() const;

        [[nodiscard]] HostDataObject & getHostObject() { return hostDataObject; }

        [[nodiscard]] bool isLocalized() const { return localized; }

        [[nodiscard]] bool isMultiframe() const { return multiframeEnabled; }


    protected:
        void init() override;

        void cleanup() override;
    };

}
