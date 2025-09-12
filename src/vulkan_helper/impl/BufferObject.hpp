//
// Created by Merutilm on 2025-07-10.
//

#pragma once
#include "CommandPool.hpp"
#include "Fence.hpp"
#include "../handle/CoreHandler.hpp"
#include "HostDataObject.hpp"
#include "../context/BufferContext.hpp"
#include "../manage/HostDataObjectManager.hpp"
#include "../struct/BufferLock.hpp"

namespace merutilm::vkh {
    class BufferObjectAbstract : public CoreHandler {
        HostDataObject hostDataObject = nullptr;
        VkBufferUsageFlags bufferUsage;
        std::variant<BufferContext, MultiframeBufferContext> bufferContext = {};
        BufferLock bufferLock;
        bool locked = false;
        const bool multiframeEnabled;

    public:
        explicit BufferObjectAbstract(CoreRef core, HostDataObjectManager &&dataManager,
                              VkBufferUsageFlags bufferUsage, BufferLock bufferLock, bool multiframeEnabled);

        ~BufferObjectAbstract() override;

        void reloadBuffer();

        BufferObjectAbstract(const BufferObjectAbstract &) = delete;

        BufferObjectAbstract &operator=(const BufferObjectAbstract &) = delete;

        BufferObjectAbstract(BufferObjectAbstract &&) = delete;

        BufferObjectAbstract &operator=(BufferObjectAbstract &&) noexcept = delete;

        void lock(CommandPoolRef commandPool, FencePtr fence = VK_NULL_HANDLE);

        void unlock(CommandPoolRef commandPool, FencePtr fence = VK_NULL_HANDLE);

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

        [[nodiscard]] HostDataObjectRef getHostObject() const { return *hostDataObject; }

        [[nodiscard]] bool isLocked() const { return locked; }

        [[nodiscard]] bool isMultiframe() const { return multiframeEnabled; }

    protected:
        void init() override;

        void destroy() override;
    };

    using BufferObject = std::unique_ptr<BufferObjectAbstract>;
    using BufferObjectPtr = BufferObjectAbstract *;
    using BufferObjectRef = BufferObjectAbstract &;
}
