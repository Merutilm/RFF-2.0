//
// Created by Merutilm on 2026-05-11.
//

#include <vulkan_helper/engine/SharedResource.hpp>

#include "vulkan_helper/engine/wrapped/Vertex.hpp"

namespace merutilm::vkh {

    SharedResource::SharedResource(Core &core) : CoreHandler(core) { SharedResource::init(); }

    SharedResource::~SharedResource() { SharedResource::cleanup(); };

    void SharedResource::init() { createIdentityBuffer(); }

    void SharedResource::createIdentityBuffer() {

        HostDataObjectManager vertManager;
        HostDataObjectManager indexManager;

        vertManager.addArray(0, std::vector{
                                        Vertex::generate({1, 1, 0}, {1, 1, 1}, {1, 1}),
                                        Vertex::generate({1, -1, 0}, {1, 1, 1}, {1, 0}),
                                        Vertex::generate({-1, -1, 0}, {1, 1, 1}, {0, 0}),
                                        Vertex::generate({-1, 1, 0}, {1, 1, 1}, {0, 1}),
                                });
        indexManager.addArray(0, std::vector<uint32_t>{0, 1, 2, 2, 3, 0});

        vertexBufferIdentity =
                std::make_unique<VertexBuffer>(core, std::move(vertManager), BufferLocalization::UNIDIRECTIONAL, false);
        indexBufferIdentity =
                std::make_unique<IndexBuffer>(core, std::move(indexManager), BufferLocalization::UNIDIRECTIONAL, false);
        vertexBufferIdentity->update();
        indexBufferIdentity->update();

        CommandPool temp(core);
        vertexBufferIdentity->localize(temp);
        indexBufferIdentity->localize(temp);

        dummyBuffer = BufferContext::createContext(
                core, {.size = 1,
                       .usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
                       .properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT});
    }
    void SharedResource::cleanup() {
        BufferContext::destroyContext(core, dummyBuffer);
        indexBufferIdentity = nullptr;
        vertexBufferIdentity = nullptr;
    }
} // namespace merutilm::vkh
