//
// Created by Merutilm on 2025-07-15.
//

#pragma once
#include "BufferObject.hpp"

namespace merutilm::vkh {
    class Uniform final : public BufferObject {

    public:
        Uniform(Core &core, HostDataObjectManager &&manager, const BufferLocalization bufferLocalization,
                const bool multiframeEnabled) :
            BufferObject(core, std::move(manager), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, bufferLocalization,
                         multiframeEnabled) {
            Uniform::init();
        }

        ~Uniform() override { Uniform::cleanup(); }

        Uniform(const Uniform &) = delete;

        Uniform &operator=(const Uniform &) = delete;

        Uniform(Uniform &&) = delete;

        Uniform &operator=(Uniform &&) = delete;

    protected:
        void init() override {
            // no operation
        }

        void cleanup() override {
            // no operation
        }
    };


} // namespace merutilm::vkh
