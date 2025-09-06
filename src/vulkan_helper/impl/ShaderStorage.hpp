//
// Created by Merutilm on 2025-08-13.
//

#pragma once
#include "BufferObject.hpp"

namespace merutilm::vkh {
    class ShaderStorageImpl final : public BufferObjectAbstract {
    public:
        explicit ShaderStorageImpl(const CoreRef core, HostDataObjectManager &&manager, BufferLock bufferLock, bool multiframeEnabled);

        ~ShaderStorageImpl() override;

        ShaderStorageImpl(const ShaderStorageImpl &) = delete;

        ShaderStorageImpl operator=(const ShaderStorageImpl &) = delete;

        ShaderStorageImpl(ShaderStorageImpl &&) = delete;

        ShaderStorageImpl operator=(ShaderStorageImpl &&) = delete;

    private:
        void init() override;

        void destroy() override;
    };

    using ShaderStorage = std::unique_ptr<ShaderStorageImpl>;
    using ShaderStoragePtr = ShaderStorageImpl *;
    using ShaderStorageRef = ShaderStorageImpl &;
}
