//
// Created by Merutilm on 2025-05-28.
//

#pragma once
#include "../../Presets.h"
#include "../../../settings/FogSettings.h"



namespace merutilm::rff::FogPresets {
    struct Medium final : public Presets::ShaderPresets::FogPreset {
        std::string getName() const override;

        FogSettings fogSettings() const override;
    };
};

