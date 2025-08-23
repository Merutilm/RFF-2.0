//
// Created by Merutilm on 2025-05-28.
//

#pragma once
#include "../../Presets.h"
#include "../../../attr/ShdFogAttribute.h"



namespace merutilm::rff2::ShdFogPresets {

    struct Disabled final : public Presets::ShaderPresets::FogPreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdFogAttribute genFog() const override;
    };
    struct Low final : public Presets::ShaderPresets::FogPreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdFogAttribute genFog() const override;
    };
    struct Medium final : public Presets::ShaderPresets::FogPreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdFogAttribute genFog() const override;
    };
    struct High final : public Presets::ShaderPresets::FogPreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdFogAttribute genFog() const override;
    };
    struct Ultra final : public Presets::ShaderPresets::FogPreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdFogAttribute genFog() const override;
    };
};

