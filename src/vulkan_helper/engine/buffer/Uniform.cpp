//
// Created by Merutilm on 2025-07-15.
//

#include <vulkan_helper/engine/buffer/Uniform.hpp>

namespace merutilm::vkh {
    Uniform::Uniform(Core & core, HostDataObjectManager &&manager, const BufferLock bufferLock, const bool multiframeEnabled) : BufferObject(core, std::move(manager), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, bufferLock, multiframeEnabled){
        Uniform::init();
    }

    Uniform::~Uniform() {
        Uniform::cleanup();
    }

    void Uniform::init() {
        //nothing to do
    }

    void Uniform::cleanup() {
        //nothing to do
    }
}
