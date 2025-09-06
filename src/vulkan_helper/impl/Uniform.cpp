//
// Created by Merutilm on 2025-07-15.
//

#include "Uniform.hpp"

namespace merutilm::vkh {
    UniformImpl::UniformImpl(const CoreRef core, HostDataObjectManager &&manager, const BufferLock bufferLock, const bool multiframeEnabled) : BufferObjectAbstract(core, std::move(manager), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, bufferLock, multiframeEnabled){
        UniformImpl::init();
    }

    UniformImpl::~UniformImpl() {
        UniformImpl::destroy();
    }

    void UniformImpl::init() {
        //nothing to do
    }

    void UniformImpl::destroy() {
        //nothing to do
    }
}
