//
// Created by Merutilm on 9/4/26.
//

#pragma once
#include "../../Presets.hpp"

namespace merutilm::rff2::RndComputePresets {
    struct None final : Presets::RenderPresets::ComputeShaderPreset {
        std::string getName() const override;
        RndComputeShader genComputeShader() const override;
    };

    struct General final : Presets::RenderPresets::ComputeShaderPreset {
        std::string getName() const override;
        RndComputeShader genComputeShader() const override;
    };

    struct LightZoomSpirals final : Presets::RenderPresets::ComputeShaderPreset {
        std::string getName() const override;
        RndComputeShader genComputeShader() const override;
    };

    struct DeepZoomSpirals final : Presets::RenderPresets::ComputeShaderPreset {
        std::string getName() const override;
        RndComputeShader genComputeShader() const override;
    };
}