//
// Created by Merutilm on 2025-05-28.
//

#pragma once
#include "../../Presets.h"
#include "../../../settings/StripeSettings.h"



namespace merutilm::rff::StripePresets {
    struct SlowAnimated final : Presets::ShaderPresets::StripePreset {
        std::string getName() const override;

        StripeSettings stripeSettings() const override;
    };
}