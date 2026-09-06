//
// Created by Merutilm on 2025-08-13.
//

#pragma once
#include "BufferObject.hpp"

namespace merutilm::vkh {
    class ShaderStorage final : public BufferObject {
    public:
        ShaderStorage(Core &core, HostDataObjectManager &&manager, const BufferLocalization bufferLocalization,
                      const bool multiframeEnabled) :
            BufferObject(core, std::move(manager), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, bufferLocalization,
                         multiframeEnabled) {
            ShaderStorage::init();
        }

        ~ShaderStorage() override { ShaderStorage::cleanup(); }

        ShaderStorage(const ShaderStorage &) = delete;

        ShaderStorage operator=(const ShaderStorage &) = delete;

        ShaderStorage(ShaderStorage &&) = delete;

        ShaderStorage operator=(ShaderStorage &&) = delete;

    protected:
        void init() override {
            // no operation
        }


        void cleanup() override {
            // no operation
        }
    };


} // namespace merutilm::vkh
