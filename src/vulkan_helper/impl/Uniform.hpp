//
// Created by Merutilm on 2025-07-15.
//

#pragma once
#include "BufferObject.hpp"

namespace merutilm::vkh {
    class UniformImpl final : public BufferObjectAbstract {

    public:
        explicit UniformImpl(const CoreRef core, HostDataObjectManager &&manager, BufferLock bufferLock, bool multiframeEnabled);

        ~UniformImpl() override;

        UniformImpl(const UniformImpl &) = delete;

        UniformImpl &operator=(const UniformImpl &) = delete;

        UniformImpl(UniformImpl &&) = delete;

        UniformImpl &operator=(UniformImpl &&) = delete;

    private:
        void init() override;

        void destroy() override;
    };

    using Uniform = std::unique_ptr<UniformImpl>;
    using UniformPtr = UniformImpl *;
    using UniformRef = UniformImpl &;
}
