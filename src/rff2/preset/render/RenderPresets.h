//
// Created by Merutilm on 2025-05-31.
//

#pragma once
#include "../../settings/RenderSettings.h"
#include "../Presets.h"

namespace merutilm::rff2::RenderPresets {
    struct Potato final : public Presets::RenderPreset {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] RenderSettings genRender() const override;
    };

    struct Low final : public Presets::RenderPreset {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] RenderSettings genRender() const override;
    };

    struct Medium final : public Presets::RenderPreset {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] RenderSettings genRender() const override;
    };

    struct High final : public Presets::RenderPreset {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] RenderSettings genRender() const override;
    };

    struct Ultra final : public Presets::RenderPreset {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] RenderSettings genRender() const override;
    };

    struct Extreme final : public Presets::RenderPreset {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] RenderSettings genRender() const override;
    };
}
