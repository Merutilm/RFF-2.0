//
// Created by Merutilm on 2025-05-28.
//

#pragma once
#include "../../Presets.h"
#include "../../../settings/SlopeSettings.h"


struct SlopePreset : public Presets::Preset {
    ~SlopePreset() override = default;

    virtual SlopeSettings slopeSettings() = 0;
};
namespace SlopePresets {
    struct Normal final : public SlopePreset {
        std::string getName() override;

        SlopeSettings slopeSettings() override;
    };
}
