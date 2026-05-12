//
// Created by Merutilm on 2025-05-28.
//

#pragma once
#include "../../../settings/ShdColorSettings.h"
#include "../../Presets.h"

namespace merutilm::rff2::ShdColorPresets {
    struct Disabled final : public Presets::ShaderPresets::ColorPreset {
        [[nodiscard]] std::wstring getName() const override;

        [[nodiscard]] ShdColorSettings genColor() const override;
    };
    struct WeakContrast final : public Presets::ShaderPresets::ColorPreset {
        [[nodiscard]] std::wstring getName() const override;

        [[nodiscard]] ShdColorSettings genColor() const override;
    };
    struct HighContrast final : public Presets::ShaderPresets::ColorPreset {
        [[nodiscard]] std::wstring getName() const override;

        [[nodiscard]] ShdColorSettings genColor() const override;
    };
    struct Dull final : public Presets::ShaderPresets::ColorPreset {
        [[nodiscard]] std::wstring getName() const override;

        [[nodiscard]] ShdColorSettings genColor() const override;
    };
    struct Vivid final : public Presets::ShaderPresets::ColorPreset {
        [[nodiscard]] std::wstring getName() const override;

        [[nodiscard]] ShdColorSettings genColor() const override;
    };
}

