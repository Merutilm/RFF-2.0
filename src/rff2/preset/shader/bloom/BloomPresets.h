//
// Created by Merutilm on 2025-05-28.
//

#pragma once
#include "../../Presets.h"
#include "../../../settings/BloomSettings.h"


namespace merutilm::rff2::BloomPresets {
    struct Disabled final : public Presets::ShaderPresets::BloomPreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] BloomSettings bloomSettings() const override;
    };

    struct Highlighted final : public Presets::ShaderPresets::BloomPreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] BloomSettings bloomSettings() const override;
    };

    struct HighlightedStrong final : public Presets::ShaderPresets::BloomPreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] BloomSettings bloomSettings() const override;
    };

    struct Weak final : public Presets::ShaderPresets::BloomPreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] BloomSettings bloomSettings() const override;
    };

    struct Normal final : public Presets::ShaderPresets::BloomPreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] BloomSettings bloomSettings() const override;
    };

    struct Strong final : public Presets::ShaderPresets::BloomPreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] BloomSettings bloomSettings() const override;
    };
}
