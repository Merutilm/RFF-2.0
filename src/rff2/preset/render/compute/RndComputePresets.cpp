//
// Created by Merutilm on 9/4/26.
//

#include "RndComputePresets.hpp"

namespace merutilm::rff2::RndComputePresets {

    std::string None::getName() const {
        return "None";
    }
    RndComputeShader None::genComputeShader() const {
        return {
            .use = false,
            .preferredBatchDuration = 0.1f,
            .allowedGlitchPixelCount = 0,
            .completelyIgnoreMpa = false,
            .automaticAcceptMpaBatches = 0,
            .interpolateIsolated = true
        };
    }
    std::string General::getName() const {
        return "General";
    }
    RndComputeShader General::genComputeShader() const {
        return {
            .use = true,
            .preferredBatchDuration = 0.1f,
            .allowedGlitchPixelCount = 0,
            .completelyIgnoreMpa = false,
            .automaticAcceptMpaBatches = 0,
            .interpolateIsolated = true
        };
    }
    std::string LightZoomSpirals::getName() const {
        return "Light Zoom Spirals";
    }
    RndComputeShader LightZoomSpirals::genComputeShader() const {
        return {
            .use = true,
            .preferredBatchDuration = 0.1f,
            .allowedGlitchPixelCount = 0,
            .completelyIgnoreMpa = false,
            .automaticAcceptMpaBatches = 16,
            .interpolateIsolated = true
        };
    }
    std::string DeepZoomSpirals::getName() const {
        return "Deep Zoom Spirals";
    }
    RndComputeShader DeepZoomSpirals::genComputeShader() const {
        return {
            .use = true,
            .preferredBatchDuration = 0.1f,
            .allowedGlitchPixelCount = 0,
            .completelyIgnoreMpa = false,
            .automaticAcceptMpaBatches = 0,
            .interpolateIsolated = true
        };
    }
} // namespace merutilm::rff2::RndComputePresets
