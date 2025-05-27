//
// Created by Merutilm on 2025-05-28.
//

#pragma once
#include "../../Presets.h"
#include "../../../settings/FogSettings.h"


struct FogPreset : public Presets::Preset {
    ~FogPreset() override = default;

    virtual FogSettings fogSettings() = 0;
};

namespace FogPresets {
    struct Medium final : public FogPreset {
        std::string getName() override;

        FogSettings fogSettings() override;
    };
};

