//
// Created by Merutilm on 2025-07-15.
//

#pragma once
#include "BufferObject.hpp"

namespace merutilm::vkh {
    class Uniform final : public BufferObject {

    public:
        explicit Uniform(const Core &core, std::unique_ptr<HostBufferObjectManager> &&manager, BufferLock bufferLock);

        ~Uniform() override;

        Uniform(const Uniform &) = delete;

        Uniform &operator=(const Uniform &) = delete;

        Uniform(Uniform &&) = delete;

        Uniform &operator=(Uniform &&) = delete;

    private:
        void init() override;

        void destroy() override;
    };
}
