//
// Created by Merutilm on 2025-07-10.
//

#pragma once
#include "CommandPool.hpp"
#include "../handle/CoreHandler.hpp"
#include "../manage/CompleteShaderObjectManager.hpp"
#include "../manage/ShaderObjectManager.hpp"

namespace merutilm::mvk {
    class BufferObject : public CoreHandler, public CompleteShaderObjectManager{

        VkBufferUsageFlags bufferUsage;
        std::vector<VkBuffer> buffers = {};
        std::vector<VkDeviceMemory> bufferMemory = {};
        std::vector<void *> bufferMapped = {};
        bool finalized = false;
        bool allInitialized = false;

    public:
        explicit BufferObject(const Core &core, std::unique_ptr<ShaderObjectManager> &&dataManager, VkBufferUsageFlags bufferUsage);

        ~BufferObject() override;

        BufferObject(const BufferObject &) = delete;

        BufferObject &operator=(const BufferObject &) = delete;

        BufferObject(BufferObject &&) = delete;

        BufferObject &operator=(BufferObject &&) noexcept = delete;

        void finalize(const CommandPool &commandPool);

        [[nodiscard]] VkBuffer getBufferHandle(const uint32_t frameIndex) const { return buffers[frameIndex]; }


        void update(uint32_t frameIndex);

        void update(uint32_t frameIndex, uint32_t target) const;

        void checkFinalizedBeforeUpdate() const;

    protected:
        void init() override;

        void destroy() override;
    };
}
