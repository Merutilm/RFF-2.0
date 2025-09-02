//
// Created by Merutilm on 2025-07-18.
//

#pragma once
#include "BufferObject.hpp"

namespace merutilm::vkh {
    class IndexBufferImpl final : public BufferObjectAbstract {

    public:
        explicit IndexBufferImpl(CoreRef core, HostDataObjectManager &&manager, BufferLock bufferLock);

        ~IndexBufferImpl() override;

        IndexBufferImpl(const IndexBufferImpl &) = delete;

        IndexBufferImpl &operator=(const IndexBufferImpl &) = delete;

        IndexBufferImpl(IndexBufferImpl &&) = delete;

        IndexBufferImpl &operator=(IndexBufferImpl &&) = delete;

        void bind(VkCommandBuffer cbh, uint32_t frameIndex, uint32_t binding) const;

    private:

        void init() override;

        void destroy() override;
    };

    using IndexBuffer = std::unique_ptr<IndexBufferImpl>;
    using IndexBufferPtr = IndexBufferImpl *;
    using IndexBufferRef = IndexBufferImpl &;
};
