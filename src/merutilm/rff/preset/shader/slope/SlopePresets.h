//
// Created by Merutilm on 2025-05-28.
//

#pragma once
#include "../../Presets.h"
#include "../../../settings/SlopeSettings.h"


namespace SlopePresets {
    struct Normal final : public Presets::ShaderPresets::SlopePreset {
        std::string getName() const override;

        SlopeSettings slopeSettings() const override;
    };
}
