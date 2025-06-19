//
// Created by Merutilm on 2025-05-28.
//

#pragma once
#include "../../Presets.h"
#include "../../../settings/FogSettings.h"



namespace merutilm::rff::FogPresets {
    struct Medium final : public Presets::ShaderPresets::FogPreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] FogSettings fogSettings() const override;
    };
};

