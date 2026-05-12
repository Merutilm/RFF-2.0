//
// Created by Merutilm on 2025-05-27.
//

#pragma once

#include "../../../settings/ShdPaletteSettings.h"
#include "../../Presets.h"


namespace merutilm::rff2::ShdPalettePresets {


    struct Classic1 final : public Presets::ShaderPresets::PalettePreset {
        [[nodiscard]] std::wstring getName() const override;

        [[nodiscard]] ShdPaletteSettings genPalette() const override;
    };

    struct Classic2 final : public Presets::ShaderPresets::PalettePreset {
        [[nodiscard]] std::wstring getName() const override;

        [[nodiscard]] ShdPaletteSettings genPalette() const override;
    };

    struct Azure final : public Presets::ShaderPresets::PalettePreset {
        [[nodiscard]] std::wstring getName() const override;

        [[nodiscard]] ShdPaletteSettings genPalette() const override;
    };

    struct Cinematic final : public Presets::ShaderPresets::PalettePreset {
        [[nodiscard]] std::wstring getName() const override;

        [[nodiscard]] ShdPaletteSettings genPalette() const override;
    };

    struct Desert final : public Presets::ShaderPresets::PalettePreset {
        [[nodiscard]] std::wstring getName() const override;

        [[nodiscard]] ShdPaletteSettings genPalette() const override;
    };


    struct Flame final : public Presets::ShaderPresets::PalettePreset {
        [[nodiscard]] std::wstring getName() const override;

        [[nodiscard]] ShdPaletteSettings genPalette() const override;
    };



    struct LongRandom64 final : public Presets::ShaderPresets::PalettePreset {
        [[nodiscard]] std::wstring getName() const override;

        [[nodiscard]] ShdPaletteSettings genPalette() const override;
    };

    struct LongRainbow7 final : public Presets::ShaderPresets::PalettePreset {
        [[nodiscard]] std::wstring getName() const override;

        [[nodiscard]] ShdPaletteSettings genPalette() const override;
    };


    struct Rainbow final : public Presets::ShaderPresets::PalettePreset {
        [[nodiscard]] std::wstring getName() const override;

        [[nodiscard]] ShdPaletteSettings genPalette() const override;
    };
}
