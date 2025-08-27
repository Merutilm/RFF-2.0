//
// Created by Merutilm on 2025-07-10.
//

#pragma once
#include "CommandPool.hpp"
#include "../handle/CoreHandler.hpp"
#include "HostBufferObject.hpp"
#include "../manage/HostBufferObjectManager.hpp"
#include "../struct/BufferLock.hpp"

namespace merutilm::vkh {
    class BufferObjectAbstract : public CoreHandler {
        std::unique_ptr<HostBufferObject> hostBufferObject = nullptr;
        VkBufferUsageFlags bufferUsage;
        std::vector<VkBuffer> buffers = {};
        std::vector<VkDeviceMemory> bufferMemory = {};
        std::vector<void *> bufferMapped = {};
        BufferLock bufferLock;
        bool locked = false;

    public:
        explicit BufferObjectAbstract(const CoreRef core, HostBufferObjectManager &&dataManager,
                              VkBufferUsageFlags bufferUsage, BufferLock bufferLock);

        ~BufferObjectAbstract() override;

        void reloadBuffer();

        BufferObjectAbstract(const BufferObjectAbstract &) = delete;

        BufferObjectAbstract &operator=(const BufferObjectAbstract &) = delete;

        BufferObjectAbstract(BufferObjectAbstract &&) = delete;

        BufferObjectAbstract &operator=(BufferObjectAbstract &&) noexcept = delete;

        void lock(CommandPoolRef commandPool);

        void unlock(CommandPoolRef commandPool);

        [[nodiscard]] VkBuffer getBufferHandle(const uint32_t frameIndex) const { return buffers[frameIndex]; }


        void update(uint32_t frameIndex) const;

        void update(uint32_t frameIndex, uint32_t target) const;

        void checkFinalizedBeforeUpdate() const;

        HostBufferObject &getHostObject() const { return *hostBufferObject; }

    protected:
        void init() override;

        void destroy() override;
    };

    using BufferObject = std::unique_ptr<BufferObjectAbstract>;
    using BufferObjectPtr = BufferObjectAbstract *;
    using BufferObjectRef = BufferObjectAbstract &;
}
