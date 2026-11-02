//
// Created by Merutilm on 2025-05-31.
//

#pragma once
#include "../../../settings/RndDisplaySettings.hpp"
#include "../../Presets.hpp"

namespace merutilm::rff2::RndDisplayPresets {
    struct Potato final : Presets::RenderPresets::DisplayPreset {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] RndDisplaySettings genDisplay() const override;
    };

    struct Low final : Presets::RenderPresets::DisplayPreset {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] RndDisplaySettings genDisplay() const override;
    };

    struct Medium final : Presets::RenderPresets::DisplayPreset {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] RndDisplaySettings genDisplay() const override;
    };

    struct High final : Presets::RenderPresets::DisplayPreset {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] RndDisplaySettings genDisplay() const override;
    };

    struct Ultra final : Presets::RenderPresets::DisplayPreset {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] RndDisplaySettings genDisplay() const override;
    };

    struct Extreme final : Presets::RenderPresets::DisplayPreset {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] RndDisplaySettings genDisplay() const override;
    };
}
