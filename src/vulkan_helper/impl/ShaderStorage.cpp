//
// Created by Merutilm on 2025-08-13.
//

#include "ShaderStorage.hpp"

namespace merutilm::vkh {

    ShaderStorageImpl::ShaderStorageImpl(const CoreRef core, HostBufferObjectManager &&manager, const BufferLock bufferLock) : BufferObjectAbstract(core, std::move(manager), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, bufferLock){
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