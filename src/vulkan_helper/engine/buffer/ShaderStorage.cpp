//
// Created by Merutilm on 2025-08-13.
//

#include <vulkan_helper/engine/buffer/ShaderStorage.hpp>

namespace merutilm::vkh {

    ShaderStorage::ShaderStorage(Core & core, HostDataObjectManager &&manager, const BufferLock bufferLock, const bool multiframeEnabled) : BufferObject(core, std::move(manager), VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, bufferLock, multiframeEnabled){
        ShaderStorage::init();
    }

    ShaderStorage::~ShaderStorage() {
        ShaderStorage::cleanup();
    }

    void ShaderStorage::init() {
        // no operation
    }


    void ShaderStorage::cleanup() {
        //no operation
    }




}