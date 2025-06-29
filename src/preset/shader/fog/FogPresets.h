//
// Created by Merutilm on 2025-05-28.
//

#pragma once
#include "../../Presets.h"
#include "../../../settings/FogSettings.h"



namespace merutilm::rff2::FogPresets {

    struct Disabled final : public Presets::ShaderPresets::FogPreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] FogSettings fogSettings() const override;
    };

    struct Medium final : public Presets::ShaderPresets::FogPreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] FogSettings fogSettings() const override;
    };
};

