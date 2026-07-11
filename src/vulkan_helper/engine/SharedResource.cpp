//
// Created by Merutilm on 2026-05-11.
//

#include <vulkan_helper/engine/SharedResource.hpp>

#include "vulkan_helper/engine/wrapped/Vertex.hpp"

namespace merutilm::vkh {

    SharedResource::SharedResource(Core &core) : CoreHandler(core) {
        SharedResource::init();
    }

    SharedResource::~SharedResource() {
        SharedResource::cleanup();
    };

    void SharedResource::init() {
        createIdentityBuffer();
    }

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

        vertexBufferIdentity = std::make_unique<VertexBuffer>(core, std::move(vertManager), BufferLock::LOCK_ONLY, false);
        indexBufferIdentity = std::make_unique<IndexBuffer>(core, std::move(indexManager), BufferLock::LOCK_ONLY, false);
        vertexBufferIdentity->update();
        indexBufferIdentity->update();

        CommandPool temp(core);
        vertexBufferIdentity->lock(temp);
        indexBufferIdentity->lock(temp);

    }
    void SharedResource::cleanup() {
        indexBufferIdentity = nullptr;
        vertexBufferIdentity = nullptr;
    }
} // namespace merutilm::vkh
