//
// Created by Merutilm on 2025-05-28.
//

#pragma once
#include "../../Presets.h"
#include "../../../settings/StripeSettings.h"



namespace merutilm::rff::StripePresets {
    struct Disabled final : Presets::ShaderPresets::StripePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] StripeSettings stripeSettings() const override;
    };

    struct SlowAnimated final : Presets::ShaderPresets::StripePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] StripeSettings stripeSettings() const override;
    };
}