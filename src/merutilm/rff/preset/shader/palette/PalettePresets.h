//
// Created by Merutilm on 2025-05-27.
//

#pragma once

#include "../../Presets.h"
#include "../../../settings/PaletteSettings.h"


struct PalettePreset : public Presets::Preset {
    ~PalettePreset() override = default;

    virtual PaletteSettings paletteSettings() = 0;
};

namespace PalettePresets {
    struct LongRandom64 final : public PalettePreset {
        std::string getName() override;
        PaletteSettings paletteSettings() override;
    };
    struct Rainbow final : public PalettePreset {
        std::string getName() override;
        PaletteSettings paletteSettings() override;
    };
}


