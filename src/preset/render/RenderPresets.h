//
// Created by Merutilm on 2025-05-31.
//

#pragma once
#include "../Presets.h"
#include "../../settings/RenderSettings.h"

namespace merutilm::rff::RenderPresets {
    struct Potato final : public Presets::RenderPresets {
        std::string getName() const override;
        RenderSettings renderSettings() const override;
    };

    struct Low final : public Presets::RenderPresets {
        std::string getName() const override;
        RenderSettings renderSettings() const override;
    };

    struct Medium final : public Presets::RenderPresets {
        std::string getName() const override;
        RenderSettings renderSettings() const override;
    };

    struct High final : public Presets::RenderPresets {
        std::string getName() const override;
        RenderSettings renderSettings() const override;
    };

    struct Ultra final : public Presets::RenderPresets {
        std::string getName() const override;
        RenderSettings renderSettings() const override;
    };

    struct Extreme final : public Presets::RenderPresets {
        std::string getName() const override;
        RenderSettings renderSettings() const override;
    };
}
