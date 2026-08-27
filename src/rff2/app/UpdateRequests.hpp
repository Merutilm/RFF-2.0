//
// Created by Merutilm on 2025-09-05.
//

#pragma once
#include <atomic>
#include <string>

#include "ComputeState.hpp"

namespace merutilm::rff2 {
    struct UpdateRequests {
        std::atomic<bool> defaultSettingsRequested = false;

        std::atomic<ComputeState> recomputeRequestedState = ComputeState::IDLE;

        std::mutex resizeMutex;
        std::atomic<bool> resizeRequested = false;
        VkExtent2D resizeRequestedExtent = {};

        std::atomic<bool> shaderRequested = false;

        std::mutex createImageMutex;
        std::atomic<bool> createImageRequested = false;
        std::string createImageRequestedFilename;

        void requestDefaultSettings() {
            defaultSettingsRequested = true;
        }


        void requestShader() {
            shaderRequested = true;
        }

        void requestResize(const VkExtent2D size) {
            std::scoped_lock lock(resizeMutex);
            resizeRequested = true;
            resizeRequestedExtent = size;
        }


        void requestRecompute() {
            recomputeRequestedState = ComputeState::REQUESTED;
        }

        void requestCreateImage(const std::string_view filename = "") {

            std::scoped_lock lock(createImageMutex);
            createImageRequestedFilename = filename;
            createImageRequested = true;
        }
    };
}
