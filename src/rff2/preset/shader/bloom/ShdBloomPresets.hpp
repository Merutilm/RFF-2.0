//
// Created by Merutilm on 2025-05-28.
//

#pragma once
#include "../../../settings/ShdBloomSettings.h"
#include "../../Presets.hpp"


namespace merutilm::rff2::ShdBloomPresets {
    struct Disabled final : Presets::ShaderPresets::BloomPreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdBloomSettings genBloom() const override;
    };

    struct Highlighted final : Presets::ShaderPresets::BloomPreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdBloomSettings genBloom() const override;
    };

    struct HighlightedStrong final : Presets::ShaderPresets::BloomPreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdBloomSettings genBloom() const override;
    };

    struct Weak final : Presets::ShaderPresets::BloomPreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdBloomSettings genBloom() const override;
    };

    struct Normal final : Presets::ShaderPresets::BloomPreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdBloomSettings genBloom() const override;
    };

    struct Strong final : Presets::ShaderPresets::BloomPreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdBloomSettings genBloom() const override;
    };
}
