//
// Created by Merutilm on 2025-05-28.
//

#pragma once
#include "../../Presets.h"
#include "../../../settings/BloomSettings.h"



namespace BloomPresets {
    struct Normal final : public Presets::ShaderPresets::BloomPreset {
        std::string getName() const override;

        BloomSettings bloomSettings() const override;
    };
}
