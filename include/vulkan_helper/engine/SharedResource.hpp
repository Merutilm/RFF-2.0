//
// Created by Merutilm on 2026-05-11.
//

#pragma once
#include <memory>

#include "buffer/IndexBuffer.hpp"
#include "buffer/VertexBuffer.hpp"
namespace merutilm::vkh {

    struct SharedResource final : CoreHandler {
        std::unique_ptr<VertexBuffer> vertexBufferIdentity;
        std::unique_ptr<IndexBuffer> indexBufferIdentity;

        explicit SharedResource(Core &core);

        ~SharedResource() override;
        SharedResource(const SharedResource &) = delete;
        SharedResource &operator=(const SharedResource &) = delete;
        SharedResource(SharedResource &&) = delete;
        SharedResource &operator=(SharedResource &&) = delete;

    protected:
        void init() override;

        void cleanup() override;

    private:
        void createIdentityBuffer();
    };

} // namespace merutilm::vkh
