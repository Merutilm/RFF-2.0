//
// Created by Merutilm on 2025-09-05.
//

#pragma once
#include <atomic>
#include <string>

namespace merutilm::rff2 {
    struct AppRenderManagerRequests {
        std::atomic<bool> defaultSettingsRequested = false;
        std::atomic<bool> recomputeRequested = false;
        std::atomic<bool> resizeRequested = false;
        VkExtent2D resizeRequestedExtent = {};

        std::atomic<bool> shaderRequested = false;
        std::atomic<bool> createImageRequested = false;
        std::string createImageRequestedFilename;

        void requestDefaultSettings() {
            defaultSettingsRequested = true;
        }


        void requestShader() {
            shaderRequested = true;
        }

        void requestResize(const VkExtent2D size) {
            resizeRequested = true;
            resizeRequestedExtent = size;
        }


        void requestRecompute() {
            recomputeRequested = true;
        }

        void requestCreateImage(const std::string_view filename = "") {
            createImageRequestedFilename = filename;
            createImageRequested = true;
        }
    };
}
