//
// Created by Merutilm on 2025-05-28.
//

#pragma once
#include "../../../settings/ShdStripeSettings.h"
#include "../../Presets.h"


namespace merutilm::rff2::ShdStripePresets {
    struct Disabled final : Presets::ShaderPresets::StripePreset {
        [[nodiscard]] std::wstring getName() const override;

        [[nodiscard]] ShdStripeSettings genStripe() const override;
    };

    struct SlowAnimated final : Presets::ShaderPresets::StripePreset {
        [[nodiscard]] std::wstring getName() const override;

        [[nodiscard]] ShdStripeSettings genStripe() const override;
    };
    struct FastAnimated final : Presets::ShaderPresets::StripePreset {
        [[nodiscard]] std::wstring getName() const override;

        [[nodiscard]] ShdStripeSettings genStripe() const override;
    };
    struct Smooth final : Presets::ShaderPresets::StripePreset {
        [[nodiscard]] std::wstring getName() const override;

        [[nodiscard]] ShdStripeSettings genStripe() const override;
    };
    struct SmoothTranslucent final : Presets::ShaderPresets::StripePreset {
        [[nodiscard]] std::wstring getName() const override;

        [[nodiscard]] ShdStripeSettings genStripe() const override;
    };
}