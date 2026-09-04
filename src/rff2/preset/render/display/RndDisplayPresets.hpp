//
// Created by Merutilm on 2025-05-31.
//

#pragma once
#include "../../../settings/RndDisplaySettings.hpp"
#include "../../Presets.hpp"

namespace merutilm::rff2::RndDisplayPresets {
    struct Potato final : public Presets::RenderPresets::DisplayPreset {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] RndDisplaySettings genDisplay() const override;
    };

    struct Low final : public Presets::RenderPresets::DisplayPreset {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] RndDisplaySettings genDisplay() const override;
    };

    struct Medium final : public Presets::RenderPresets::DisplayPreset {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] RndDisplaySettings genDisplay() const override;
    };

    struct High final : public Presets::RenderPresets::DisplayPreset {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] RndDisplaySettings genDisplay() const override;
    };

    struct Ultra final : public Presets::RenderPresets::DisplayPreset {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] RndDisplaySettings genDisplay() const override;
    };

    struct Extreme final : public Presets::RenderPresets::DisplayPreset {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] RndDisplaySettings genDisplay() const override;
    };
}
