//
// Created by Merutilm on 2025-05-28.
//

#pragma once
#include "../../../settings/ShdColorSettings.h"
#include "../../Presets.hpp"

namespace merutilm::rff2::ShdColorPresets {
    struct Disabled final : Presets::ShaderPresets::ColorPreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdColorSettings genColor() const override;
    };
    struct WeakContrast final : Presets::ShaderPresets::ColorPreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdColorSettings genColor() const override;
    };
    struct HighContrast final : Presets::ShaderPresets::ColorPreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdColorSettings genColor() const override;
    };
    struct Dull final : Presets::ShaderPresets::ColorPreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdColorSettings genColor() const override;
    };
    struct Vivid final : Presets::ShaderPresets::ColorPreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdColorSettings genColor() const override;
    };
}

