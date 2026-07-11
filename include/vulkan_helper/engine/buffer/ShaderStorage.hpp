//
// Created by Merutilm on 2025-08-13.
//

#pragma once
#include "BufferObject.hpp"

namespace merutilm::vkh {
    class ShaderStorage final : public BufferObject {
    public:
        explicit ShaderStorage(Core & core, HostDataObjectManager &&manager, BufferLock bufferLock, bool multiframeEnabled);

        ~ShaderStorage() override;

        ShaderStorage(const ShaderStorage &) = delete;

        ShaderStorage operator=(const ShaderStorage &) = delete;

        ShaderStorage(ShaderStorage &&) = delete;

        ShaderStorage operator=(ShaderStorage &&) = delete;

    protected:
        void init() override;

        void cleanup() override;
    };


}
