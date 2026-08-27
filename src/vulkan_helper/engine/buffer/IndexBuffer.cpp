//
// Created by Merutilm on 2025-07-18.
//

#include <vulkan_helper/engine/buffer/IndexBuffer.hpp>

namespace merutilm::vkh {
    IndexBuffer::IndexBuffer(Core & core, HostDataObjectManager &&manager, const BufferLocalization bufferLocalization, const bool multiframeEnabled) : BufferObject(
        core, std::move(manager), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, bufferLocalization, multiframeEnabled) {
        IndexBuffer::init();
    }

    IndexBuffer::~IndexBuffer() {
        IndexBuffer::cleanup();
    }


    void IndexBuffer::init() {
        //no operation
    }

    void IndexBuffer::cleanup() {
        //no operation
    }
}
