//
// Created by Merutilm on 2025-07-18.
//

#include "IndexBuffer.hpp"

namespace merutilm::vkh {
    IndexBufferImpl::IndexBufferImpl(CoreRef core, HostDataObjectManager &&manager, const BufferLock bufferLock, const bool multiframeEnabled) : BufferObjectAbstract(
        core, std::move(manager), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, bufferLock, multiframeEnabled) {
        IndexBufferImpl::init();
    }

    IndexBufferImpl::~IndexBufferImpl() {
        IndexBufferImpl::destroy();
    }


    void IndexBufferImpl::init() {
        //no operation
    }

    void IndexBufferImpl::destroy() {
        //no operation
    }
}
