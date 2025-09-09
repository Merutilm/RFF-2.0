//
// Created by Merutilm on 2025-09-05.
//

#pragma once
#include <atomic>
#include <string>

namespace merutilm::rff2 {
    struct RenderSceneRequests {
        std::atomic<bool> defaultAttrRequested = false;
        std::atomic<bool> recomputeRequested = false;
        std::atomic<bool> resizeRequested = false;
        std::atomic<bool> shaderRequested = false;
        std::atomic<bool> createImageRequested = false;
        std::string createImageRequestedFilename;

        void requestDefaultSettings() {
            defaultAttrRequested = true;
        };

        void requestShader() {
            shaderRequested = true;
        }

        void requestResize() {
            resizeRequested = true;
        }

        void requestRecompute() {
            recomputeRequested = true;
        }

        void requestCreateImage(const std::string_view filename = "") {
            createImageRequested = true;
            createImageRequestedFilename = filename;
        }

    };
}
