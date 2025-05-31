//
// Created by Merutilm on 2025-05-27.
//

#pragma once

#include "../../Presets.h"
#include "../../../settings/PaletteSettings.h"


namespace PalettePresets {
    struct LongRandom64 final : public Presets::ShaderPresets::PalettePreset {
        std::string getName() const override;
        PaletteSettings paletteSettings() const override;
    };
    struct Rainbow final : public Presets::ShaderPresets::PalettePreset {
        std::string getName() const override;
        PaletteSettings paletteSettings() const override;
    };
}


