//
// Created by Merutilm on 2026-02-03.
//
#include <vulkan_helper/engine/context/SharedImageContext.hpp>

namespace merutilm::vkh {
    SharedImageContext::SharedImageContext(Core & core): core(core) {
    }

    SharedImageContext::~SharedImageContext() {
        cleanupContexts();
    }

    void SharedImageContext::appendMultiframeImageContext(const uint32_t expected, ImageInitInfo &&iii) {
        safe_array::check_index_equal(expected, multiframeContexts.size(),
                                      "Shared Multiframe Image Context append");
        multiframeContexts.emplace_back(ImageContext::createMultiframeContext(core, iii));
    }

    void SharedImageContext::appendImageContext(const uint32_t expected, ImageInitInfo &&iii) {
        safe_array::check_index_equal(expected, imageContexts.size(), "Shared Image Context append");
        imageContexts.emplace_back(ImageContext::createContext(core, iii));
    }

    void SharedImageContext::cleanupContexts() {
        for (const auto &context: multiframeContexts) {
            ImageContext::destroyContext(core, context);
        }
        for (const auto &context: imageContexts) {
            ImageContext::destroyContext(core, context);
        }
        multiframeContexts.clear();
        imageContexts.clear();
    }
}
