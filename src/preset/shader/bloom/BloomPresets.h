//
// Created by Merutilm on 2025-05-28.
//

#pragma once
#include "../../Presets.h"
#include "../../../settings/BloomSettings.h"


namespace merutilm::rff::BloomPresets {
    struct Normal final : public Presets::ShaderPresets::BloomPreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] BloomSettings bloomSettings() const override;
    };
}
