//
// Created by Merutilm on 2025-07-15.
//

#include "VertexBuffer.hpp"

#include "../manage/ShaderObjectManager.hpp"
#include "../struct/Vertex.hpp"
#include "../util/BufferImageUtils.hpp"

namespace merutilm::vkh {
    VertexBuffer::VertexBuffer(const Core &core, std::unique_ptr<ShaderObjectManager> &&manager) : BufferObject(
        core, std::move(manager), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT) {
        VertexBuffer::init();
    }

    VertexBuffer::~VertexBuffer() {
        VertexBuffer::destroy();
    }


    void VertexBuffer::init() {
        const uint32_t len = getObjectCount();
        for (uint32_t i = 0; i < len; ++i) {
            const uint32_t size = sizes[i];
            const uint32_t offset = offsets[i];
            if (const uint32_t element = elements[i]; size != sizeof(Vertex) * element) {
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

    void VertexBuffer::destroy() {
        //nothing to do
    }
}
