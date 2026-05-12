//
// Created by Merutilm on 2025-05-28.
//

#pragma once
#include "../../../settings/ShdBloomSettings.h"
#include "../../Presets.h"


namespace merutilm::rff2::BloomPresets {
    struct Disabled final : public Presets::ShaderPresets::BloomPreset {
        [[nodiscard]] std::wstring getName() const override;

        [[nodiscard]] ShdBloomSettings genBloom() const override;
    };

    struct Highlighted final : public Presets::ShaderPresets::BloomPreset {
        [[nodiscard]] std::wstring getName() const override;

        [[nodiscard]] ShdBloomSettings genBloom() const override;
    };

    struct HighlightedStrong final : public Presets::ShaderPresets::BloomPreset {
        [[nodiscard]] std::wstring getName() const override;

        [[nodiscard]] ShdBloomSettings genBloom() const override;
    };

    struct Weak final : public Presets::ShaderPresets::BloomPreset {
        [[nodiscard]] std::wstring getName() const override;

        [[nodiscard]] ShdBloomSettings genBloom() const override;
    };

    struct Normal final : public Presets::ShaderPresets::BloomPreset {
        [[nodiscard]] std::wstring getName() const override;

        [[nodiscard]] ShdBloomSettings genBloom() const override;
    };

    struct Strong final : public Presets::ShaderPresets::BloomPreset {
        [[nodiscard]] std::wstring getName() const override;

        [[nodiscard]] ShdBloomSettings genBloom() const override;
    };
}
