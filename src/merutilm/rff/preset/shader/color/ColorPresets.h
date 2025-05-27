//
// Created by Merutilm on 2025-05-28.
//

#pragma once
#include "../../Presets.h"
#include "../../../settings/ColorSettings.h"

struct ColorPreset : public Presets::Preset {
    ~ColorPreset() override = default;

    virtual ColorSettings colorSettings() = 0;
};
namespace ColorPresets {
    struct WeakContrast final : public ColorPreset {
        std::string getName() override;

        ColorSettings colorSettings() override;
    };
}

