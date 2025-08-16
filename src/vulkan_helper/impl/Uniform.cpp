//
// Created by Merutilm on 2025-07-15.
//

#include "Uniform.hpp"

namespace merutilm::vkh {
    Uniform::Uniform(const Core &core, HostBufferObjectManager &&manager, const BufferLock bufferLock) : BufferObject(core, std::move(manager), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, bufferLock){
        Uniform::init();
    }

    Uniform::~Uniform() {
        Uniform::destroy();
    }

    void Uniform::init() {
        //nothing to do
    }

    void Uniform::destroy() {
        //nothing to do
    }
}
