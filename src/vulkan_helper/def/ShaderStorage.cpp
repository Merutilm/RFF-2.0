//
// Created by Merutilm on 2025-08-13.
//

#include "ShaderStorage.hpp"

namespace merutilm::vkh {

    ShaderStorage::ShaderStorage(const Core &core, std::unique_ptr<HostBufferObjectManager> &&manager, const BufferLock bufferLock) : BufferObject(core, std::move(manager), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, bufferLock){
        ShaderStorage::init();
    }

    ShaderStorage::~ShaderStorage() {
        ShaderStorage::destroy();
    }

    void ShaderStorage::init() {
        // no operation
    }


    void ShaderStorage::destroy() {
        //no operation
    }




}