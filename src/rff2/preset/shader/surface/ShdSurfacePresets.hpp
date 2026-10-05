//
// Created by Merutilm on 2025-05-28.
//

#pragma once
#include "../../../settings/ShdSurfaceSettings.h"
#include "../../Presets.hpp"


namespace merutilm::rff2::ShdSurfacePresets {

    struct Disabled final : public Presets::ShaderPresets::SurfacePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdSurfaceSettings genSurface() const override;
    };

    struct HighContrast final : public Presets::ShaderPresets::SurfacePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdSurfaceSettings genSurface() const override;
    };

    struct Reflective final : public Presets::ShaderPresets::SurfacePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdSurfaceSettings genSurface() const override;
    };


    struct Translucent final : public Presets::ShaderPresets::SurfacePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdSurfaceSettings genSurface() const override;
    };
    struct Reversed final : public Presets::ShaderPresets::SurfacePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdSurfaceSettings genSurface() const override;
    };

    struct Micro final : public Presets::ShaderPresets::SurfacePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdSurfaceSettings genSurface() const override;
    };

    struct Nano final : public Presets::ShaderPresets::SurfacePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdSurfaceSettings genSurface() const override;
    };
}
