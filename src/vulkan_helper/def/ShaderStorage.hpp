//
// Created by Merutilm on 2025-08-13.
//

#pragma once
#include "BufferObject.hpp"

namespace merutilm::vkh {
    class ShaderStorage final : public BufferObject {
    public:
        explicit ShaderStorage(const Core &core, std::unique_ptr<BufferObjectManager> &&manager, BufferLock bufferLock);

        ~ShaderStorage() override;

        ShaderStorage(const ShaderStorage &) = delete;

        ShaderStorage operator=(const ShaderStorage &) = delete;

        ShaderStorage(ShaderStorage &&) = delete;

        ShaderStorage operator=(ShaderStorage &&) = delete;

    private:
        void init() override;

        void destroy() override;
    };
}
