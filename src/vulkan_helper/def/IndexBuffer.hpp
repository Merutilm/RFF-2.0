//
// Created by Merutilm on 2025-07-18.
//

#pragma once
#include "BufferObject.hpp"

namespace merutilm::vkh {
    class IndexBuffer final : public BufferObject {

    public:
        explicit IndexBuffer(const Core &core, std::unique_ptr<BufferObjectManager> &&manager, BufferLock bufferLock);

        ~IndexBuffer() override;

        IndexBuffer(const IndexBuffer &) = delete;

        IndexBuffer &operator=(const IndexBuffer &) = delete;

        IndexBuffer(IndexBuffer &&) = delete;

        IndexBuffer &operator=(IndexBuffer &&) = delete;

        void bind(VkCommandBuffer cbh, uint32_t frameIndex, uint32_t binding) const;

    private:

        void init() override;

        void destroy() override;
    };
};
