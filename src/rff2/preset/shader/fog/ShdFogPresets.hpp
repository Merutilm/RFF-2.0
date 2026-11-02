//
// Created by Merutilm on 2025-05-28.
//

#pragma once
#include "../../../settings/ShdFogSettings.h"
#include "../../Presets.hpp"


namespace merutilm::rff2::ShdFogPresets {

    struct Disabled final : Presets::ShaderPresets::FogPreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdFogSettings genFog() const override;
    };
    struct Low final : Presets::ShaderPresets::FogPreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdFogSettings genFog() const override;
    };
    struct Medium final : Presets::ShaderPresets::FogPreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdFogSettings genFog() const override;
    };
    struct High final : Presets::ShaderPresets::FogPreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdFogSettings genFog() const override;
    };
    struct Ultra final : Presets::ShaderPresets::FogPreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdFogSettings genFog() const override;
    };
};

