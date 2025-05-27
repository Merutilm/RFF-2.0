//
// Created by Merutilm on 2025-05-28.
//

#pragma once
#include "../../Presets.h"
#include "../../../settings/StripeSettings.h"


struct StripePreset : public Presets::Preset {
    ~StripePreset() override = default;

    virtual StripeSettings stripeSettings() = 0;
};

namespace StripePresets {
    struct SlowAnimated final : StripePreset {
        std::string getName() override;

        StripeSettings stripeSettings() override;
    };
}