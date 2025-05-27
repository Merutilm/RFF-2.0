//
// Created by Merutilm on 2025-05-28.
//

#pragma once
#include "../../Presets.h"
#include "../../../settings/BloomSettings.h"


struct BloomPreset : public Presets::Preset {
    ~BloomPreset() override = default;

    virtual BloomSettings bloomSettings() = 0;
};

namespace BloomPresets {
    struct Normal final : public BloomPreset {
        std::string getName() override;

        BloomSettings bloomSettings() override;
    };
}
