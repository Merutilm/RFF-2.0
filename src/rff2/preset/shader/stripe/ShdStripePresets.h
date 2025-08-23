//
// Created by Merutilm on 2025-05-28.
//

#pragma once
#include "../../Presets.h"
#include "../../../attr/ShdStripeAttribute.h"



namespace merutilm::rff2::ShdStripePresets {
    struct Disabled final : Presets::ShaderPresets::StripePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdStripeAttribute genStripe() const override;
    };

    struct SlowAnimated final : Presets::ShaderPresets::StripePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdStripeAttribute genStripe() const override;
    };
    struct FastAnimated final : Presets::ShaderPresets::StripePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdStripeAttribute genStripe() const override;
    };
    struct Smooth final : Presets::ShaderPresets::StripePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdStripeAttribute genStripe() const override;
    };
    struct SmoothTranslucent final : Presets::ShaderPresets::StripePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdStripeAttribute genStripe() const override;
    };
}