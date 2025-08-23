//
// Created by Merutilm on 2025-05-28.
//

#pragma once
#include "../../Presets.h"
#include "../../../attr/ShdBloomAttribute.h"


namespace merutilm::rff2::BloomPresets {
    struct Disabled final : public Presets::ShaderPresets::BloomPreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdBloomAttribute genBloom() const override;
    };

    struct Highlighted final : public Presets::ShaderPresets::BloomPreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdBloomAttribute genBloom() const override;
    };

    struct HighlightedStrong final : public Presets::ShaderPresets::BloomPreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdBloomAttribute genBloom() const override;
    };

    struct Weak final : public Presets::ShaderPresets::BloomPreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdBloomAttribute genBloom() const override;
    };

    struct Normal final : public Presets::ShaderPresets::BloomPreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdBloomAttribute genBloom() const override;
    };

    struct Strong final : public Presets::ShaderPresets::BloomPreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdBloomAttribute genBloom() const override;
    };
}
