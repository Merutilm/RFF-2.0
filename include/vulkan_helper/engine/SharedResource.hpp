//
// Created by Merutilm on 2026-05-11.
//

#pragma once
#include <memory>

#include "buffer/IndexBuffer.hpp"
#include "buffer/VertexBuffer.hpp"
#include "context/WindowContext.hpp"
namespace merutilm::vkh {

    struct SharedResource final : Handler {
        WindowContext &wc;
        std::unique_ptr<VertexBuffer> vertexBufferPP;
        std::unique_ptr<IndexBuffer> indexBufferPP;

        explicit SharedResource(WindowContext &wc);

        ~SharedResource() override;
        SharedResource(const SharedResource &) = delete;
        SharedResource &operator=(const SharedResource &) = delete;
        SharedResource(SharedResource &&) = delete;
        SharedResource &operator=(SharedResource &&) = delete;

    private:
        void init() override;

        void createPPBuffer();

        void cleanup() override;
    };

} // namespace merutilm::vkh
