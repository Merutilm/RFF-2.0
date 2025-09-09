//
// Created by Merutilm on 2025-05-31.
//

#pragma once
#include "../Presets.h"
#include "../../attr/RenderAttribute.h"

namespace merutilm::rff2::RenderPresets {
    struct Potato final : public Presets::RenderPreset {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] RenderAttribute genRender() const override;
    };

    struct Low final : public Presets::RenderPreset {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] RenderAttribute genRender() const override;
    };

    struct Medium final : public Presets::RenderPreset {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] RenderAttribute genRender() const override;
    };

    struct High final : public Presets::RenderPreset {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] RenderAttribute genRender() const override;
    };

    struct Ultra final : public Presets::RenderPreset {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] RenderAttribute genRender() const override;
    };

    struct Extreme final : public Presets::RenderPreset {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] RenderAttribute genRender() const override;
    };
}
