//
// Created by Merutilm on 2025-08-13.
//

#include "ShaderStorage.hpp"

namespace merutilm::vkh {

    ShaderStorageImpl::ShaderStorageImpl(const CoreRef core, HostDataObjectManager &&manager, const BufferLock bufferLock, const bool multiframeEnabled) : BufferObjectAbstract(core, std::move(manager), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, bufferLock, multiframeEnabled){
        ShaderStorageImpl::init();
    }

    ShaderStorageImpl::~ShaderStorageImpl() {
        ShaderStorageImpl::destroy();
    }

    void ShaderStorageImpl::init() {
        // no operation
    }


    void ShaderStorageImpl::destroy() {
        //no operation
    }




}