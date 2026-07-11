//
// Created by Merutilm on 2025-07-18.
//

#pragma once
#include "BufferObject.hpp"

namespace merutilm::vkh {
    class IndexBuffer final : public BufferObject {

    public:
        explicit IndexBuffer(Core & core, HostDataObjectManager &&manager, BufferLock bufferLock, bool multiframeEnabled);

        ~IndexBuffer() override;

        IndexBuffer(const IndexBuffer &) = delete;

        IndexBuffer &operator=(const IndexBuffer &) = delete;

        IndexBuffer(IndexBuffer &&) = delete;

        IndexBuffer &operator=(IndexBuffer &&) = delete;

    protected:

        void init() override;

        void cleanup() override;
    };


};
