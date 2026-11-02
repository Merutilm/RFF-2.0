//
// Created by Merutilm on 2025-05-27.
//

#pragma once

#include "../../../settings/ShdPaletteSettings.h"
#include "../../Presets.hpp"


namespace merutilm::rff2::ShdPalettePresets {


    struct Classic1 final : Presets::ShaderPresets::PalettePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdPaletteSettings genPalette() const override;
    };

    struct Classic2 final : Presets::ShaderPresets::PalettePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdPaletteSettings genPalette() const override;
    };

    struct Azure final : Presets::ShaderPresets::PalettePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdPaletteSettings genPalette() const override;
    };

    struct Cinematic final : Presets::ShaderPresets::PalettePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdPaletteSettings genPalette() const override;
    };

    struct Desert final : Presets::ShaderPresets::PalettePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdPaletteSettings genPalette() const override;
    };


    struct Flame final : Presets::ShaderPresets::PalettePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdPaletteSettings genPalette() const override;
    };



    struct LongRandom64 final : Presets::ShaderPresets::PalettePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdPaletteSettings genPalette() const override;
    };

    struct LongRainbow7 final : Presets::ShaderPresets::PalettePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdPaletteSettings genPalette() const override;
    };


    struct Rainbow final : Presets::ShaderPresets::PalettePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdPaletteSettings genPalette() const override;
    };
}
