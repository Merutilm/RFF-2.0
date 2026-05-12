//
// Created by Merutilm on 2026-05-11.
//

#include <vulkan_helper/engine/SharedResource.hpp>

#include "vulkan_helper/engine/wrapped/Vertex.hpp"

namespace merutilm::vkh {

    SharedResource::SharedResource(WindowContext &wc) : wc(wc) {
        SharedResource::init();
    }

    SharedResource::~SharedResource() {
        SharedResource::cleanup();
    };

    void SharedResource::init() {
        createPPBuffer();
    }

    void SharedResource::createPPBuffer() {

        HostDataObjectManager vertManager;
        HostDataObjectManager indexManager;

        vertManager.addArray(0, std::vector{
                                        Vertex::generate({1, 1, 0}, {1, 1, 1}, {1, 1}),
                                        Vertex::generate({1, -1, 0}, {1, 1, 1}, {1, 0}),
                                        Vertex::generate({-1, -1, 0}, {1, 1, 1}, {0, 0}),
                                        Vertex::generate({-1, 1, 0}, {1, 1, 1}, {0, 1}),
                                });
        indexManager.addArray(0, std::vector<uint32_t>{0, 1, 2, 2, 3, 0});

        vertexBufferPP = std::make_unique<VertexBuffer>(wc.core, std::move(vertManager), BufferLock::LOCK_ONLY, false);
        indexBufferPP = std::make_unique<IndexBuffer>(wc.core, std::move(indexManager), BufferLock::LOCK_ONLY, false);
        vertexBufferPP->update();
        indexBufferPP->update();
        vertexBufferPP->lock(wc.getCommandPool());
        indexBufferPP->lock(wc.getCommandPool());
    }
    void SharedResource::cleanup() {
        indexBufferPP = nullptr;
        vertexBufferPP = nullptr;
    }
} // namespace merutilm::vkh
