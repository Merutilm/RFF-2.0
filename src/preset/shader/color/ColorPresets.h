//
// Created by Merutilm on 2025-05-28.
//

#pragma once
#include "../../Presets.h"
#include "../../../settings/ColorSettings.h"

namespace merutilm::rff::ColorPresets {
    struct WeakContrast final : public Presets::ShaderPresets::ColorPreset {
        std::string getName() const override;

        ColorSettings colorSettings() const override;
    };
}

