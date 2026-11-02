//
// Created by Merutilm on 2025-05-28.
//

#pragma once
#include "../../../settings/ShdSurfaceSettings.h"
#include "../../Presets.hpp"


namespace merutilm::rff2::ShdSurfacePresets {

    struct Disabled final : Presets::ShaderPresets::SurfacePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdSurfaceSettings genSurface() const override;
    };

    struct HighContrast final : Presets::ShaderPresets::SurfacePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdSurfaceSettings genSurface() const override;
    };

    struct Reflective final : Presets::ShaderPresets::SurfacePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdSurfaceSettings genSurface() const override;
    };


    struct Translucent final : Presets::ShaderPresets::SurfacePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdSurfaceSettings genSurface() const override;
    };
    struct Reversed final : Presets::ShaderPresets::SurfacePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdSurfaceSettings genSurface() const override;
    };

    struct Micro final : Presets::ShaderPresets::SurfacePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdSurfaceSettings genSurface() const override;
    };

    struct Nano final : Presets::ShaderPresets::SurfacePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdSurfaceSettings genSurface() const override;
    };
}
