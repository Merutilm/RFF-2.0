//
// Created by Merutilm on 2025-07-18.
//

#include "IndexBuffer.hpp"

namespace merutilm::vkh {
    IndexBufferImpl::IndexBufferImpl(CoreRef core, HostDataObjectManager &&manager, const BufferLock bufferLock) : BufferObjectAbstract(
        core, std::move(manager), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, bufferLock) {
        IndexBufferImpl::init();
    }

    IndexBufferImpl::~IndexBufferImpl() {
        IndexBufferImpl::destroy();
    }

    void IndexBufferImpl::bind(const VkCommandBuffer cbh, const uint32_t frameIndex, const uint32_t binding) const {
        vkCmdBindIndexBuffer(cbh, getBufferHandle(frameIndex), getHostObject().getOffset(binding), VK_INDEX_TYPE_UINT32);
    }


    void IndexBufferImpl::init() {
        //no operation
    }

    void IndexBufferImpl::destroy() {
        //no operation
    }
}
