//
// Created by Merutilm on 2025-07-15.
//

#include <vulkan_helper/engine/buffer/VertexBuffer.hpp>

#include <vulkan_helper/engine/manage/HostDataObjectManager.hpp>
#include <vulkan_helper/engine/wrapped/Vertex.hpp>

namespace merutilm::vkh {
    VertexBuffer::VertexBuffer(Core & core, HostDataObjectManager &&manager, const BufferLock bufferLock, bool multiframeEnabled) : BufferObject(
        core, std::move(manager), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, bufferLock, multiframeEnabled) {
        VertexBuffer::init();
    }

    VertexBuffer::~VertexBuffer() {
        VertexBuffer::cleanup();
    }


    void VertexBuffer::init() {
        const auto &host = getHostObject();
        const uint32_t len = host.getObjectCount();
        for (uint32_t i = 0; i < len; ++i) {
            const uint32_t size = host.sizes[i];
            const uint32_t offset = host.offsets[i];
            if (const uint32_t element = host.elements[i]; size != sizeof(Vertex) * element) {
                throw exception_invalid_args(std::format("size {} and {} is not match", size,
                                                         sizeof(Vertex) * element));
            }
            bindingDescriptions.emplace_back(i, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX);
            vertexInputAttributeDescriptions.emplace_back(0, i, getFormat<decltype(Vertex::position)>(),
                                                          offset + offsetof(Vertex, position));
            vertexInputAttributeDescriptions.emplace_back(1, i, getFormat<decltype(Vertex::color)>(),
            offset + offsetof(Vertex, color));
            vertexInputAttributeDescriptions.emplace_back(2, i, getFormat<decltype(Vertex::texcoord)>(),
                                                          offset + offsetof(Vertex, texcoord));
        }
    }

    void VertexBuffer::cleanup() {
        //nothing to do
    }
}
