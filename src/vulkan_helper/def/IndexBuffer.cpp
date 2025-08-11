//
// Created by Merutilm on 2025-07-18.
//

#include "IndexBuffer.hpp"

namespace merutilm::vkh {
    IndexBuffer::IndexBuffer(const Core &core, std::unique_ptr<ShaderObjectManager> &&manager) : BufferObject(
        core, std::move(manager), VK_BUFFER_USAGE_INDEX_BUFFER_BIT) {
        IndexBuffer::init();
    }

    IndexBuffer::~IndexBuffer() {
        IndexBuffer::destroy();
    }

    void IndexBuffer::bind(const VkCommandBuffer cbh, const uint32_t frameIndex, const uint32_t binding) const {
        vkCmdBindIndexBuffer(cbh, getBufferHandle(frameIndex), getOffset(binding), VK_INDEX_TYPE_UINT32);
    }


    void IndexBuffer::init() {
        //no operation
    }

    void IndexBuffer::destroy() {
        //no operation
    }
}
