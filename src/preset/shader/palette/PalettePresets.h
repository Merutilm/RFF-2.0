//
// Created by Merutilm on 2025-05-27.
//

#pragma once

#include "../../Presets.h"
#include "../../../settings/PaletteSettings.h"


namespace merutilm::rff2::PalettePresets {


    struct Classic1 final : public Presets::ShaderPresets::PalettePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] PaletteSettings paletteSettings() const override;
    };

    struct Classic2 final : public Presets::ShaderPresets::PalettePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] PaletteSettings paletteSettings() const override;
    };

    struct Azure final : public Presets::ShaderPresets::PalettePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] PaletteSettings paletteSettings() const override;
    };

    struct Cinematic final : public Presets::ShaderPresets::PalettePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] PaletteSettings paletteSettings() const override;
    };

    struct Desert final : public Presets::ShaderPresets::PalettePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] PaletteSettings paletteSettings() const override;
    };


    struct Flame final : public Presets::ShaderPresets::PalettePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] PaletteSettings paletteSettings() const override;
    };



    struct LongRandom64 final : public Presets::ShaderPresets::PalettePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] PaletteSettings paletteSettings() const override;
    };

    struct LongRainbow7 final : public Presets::ShaderPresets::PalettePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] PaletteSettings paletteSettings() const override;
    };


    struct Rainbow final : public Presets::ShaderPresets::PalettePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] PaletteSettings paletteSettings() const override;
    };
}
