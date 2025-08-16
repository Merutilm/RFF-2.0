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
    class BufferObject : public CoreHandler {
        std::unique_ptr<HostBufferObject> hostBufferObject = nullptr;
        VkBufferUsageFlags bufferUsage;
        std::vector<VkBuffer> buffers = {};
        std::vector<VkDeviceMemory> bufferMemory = {};
        std::vector<void *> bufferMapped = {};
        BufferLock bufferLock;
        bool locked = false;

    public:
        explicit BufferObject(const Core &core, std::unique_ptr<HostBufferObjectManager> &&dataManager,
                              VkBufferUsageFlags bufferUsage, BufferLock bufferLock);

        ~BufferObject() override;

        void reloadBuffer();

        BufferObject(const BufferObject &) = delete;

        BufferObject &operator=(const BufferObject &) = delete;

        BufferObject(BufferObject &&) = delete;

        BufferObject &operator=(BufferObject &&) noexcept = delete;

        void lock(const CommandPool &commandPool);

        void unlock(const CommandPool &commandPool);

        [[nodiscard]] VkBuffer getBufferHandle(const uint32_t frameIndex) const { return buffers[frameIndex]; }


        void update(uint32_t frameIndex) const;

        void update(uint32_t frameIndex, uint32_t target) const;

        void checkFinalizedBeforeUpdate() const;

        HostBufferObject &getHostObject() const { return *hostBufferObject; }

    protected:
        void init() override;

        void destroy() override;
    };
}
