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
        MultiframeBufferContext bufferContext = {};
        BufferLock bufferLock;
        bool locked = false;

    public:
        explicit BufferObjectAbstract(CoreRef core, HostDataObjectManager &&dataManager,
                              VkBufferUsageFlags bufferUsage, BufferLock bufferLock);

        ~BufferObjectAbstract() override;

        void reloadBuffer();

        BufferObjectAbstract(const BufferObjectAbstract &) = delete;

        BufferObjectAbstract &operator=(const BufferObjectAbstract &) = delete;

        BufferObjectAbstract(BufferObjectAbstract &&) = delete;

        BufferObjectAbstract &operator=(BufferObjectAbstract &&) noexcept = delete;

        void lock(CommandPoolRef commandPool, VkFence fence = VK_NULL_HANDLE);

        void unlock(CommandPoolRef commandPool, VkFence fence = VK_NULL_HANDLE);

        [[nodiscard]] const BufferContext &getBufferContext(const uint32_t frameIndex) const { return bufferContext[frameIndex]; }

        void update(uint32_t frameIndex) const;

        void update(uint32_t frameIndex, uint32_t target) const;

        void checkFinalizedBeforeUpdate() const;

        [[nodiscard]] HostDataObjectRef getHostObject() const { return *hostDataObject; }

        [[nodiscard]] bool isLocked() const { return locked; }

    protected:
        void init() override;

        void destroy() override;
    };

    using BufferObject = std::unique_ptr<BufferObjectAbstract>;
    using BufferObjectPtr = BufferObjectAbstract *;
    using BufferObjectRef = BufferObjectAbstract &;
}
