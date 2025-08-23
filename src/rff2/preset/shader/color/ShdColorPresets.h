//
// Created by Merutilm on 2025-05-28.
//

#pragma once
#include "../../Presets.h"
#include "../../../attr/ShdColorAttribute.h"

namespace merutilm::rff2::ShdColorPresets {
    struct Disabled final : public Presets::ShaderPresets::ColorPreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdColorAttribute genColor() const override;
    };
    struct WeakContrast final : public Presets::ShaderPresets::ColorPreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdColorAttribute genColor() const override;
    };
    struct HighContrast final : public Presets::ShaderPresets::ColorPreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdColorAttribute genColor() const override;
    };
    struct Dull final : public Presets::ShaderPresets::ColorPreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdColorAttribute genColor() const override;
    };
    struct Vivid final : public Presets::ShaderPresets::ColorPreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdColorAttribute genColor() const override;
    };
}

