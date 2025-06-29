//
// Created by Merutilm on 2025-05-28.
//

#pragma once
#include "../../Presets.h"
#include "../../../settings/ColorSettings.h"

namespace merutilm::rff2::ColorPresets {
    struct WeakContrast final : public Presets::ShaderPresets::ColorPreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ColorSettings colorSettings() const override;
    };
}

