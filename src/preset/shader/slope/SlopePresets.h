//
// Created by Merutilm on 2025-05-28.
//

#pragma once
#include "../../Presets.h"
#include "../../../settings/SlopeSettings.h"


namespace merutilm::rff::SlopePresets {

    struct Disabled final : public Presets::ShaderPresets::SlopePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] SlopeSettings slopeSettings() const override;
    };

    struct Normal final : public Presets::ShaderPresets::SlopePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] SlopeSettings slopeSettings() const override;
    };
}
