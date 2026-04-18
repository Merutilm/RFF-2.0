//
// Created by Merutilm on 2025-05-28.
//

#pragma once
#include "../../../settings/ShdSlopeSettings.h"
#include "../../Presets.h"


namespace merutilm::rff2::ShdSlopePresets {

    struct Disabled final : public Presets::ShaderPresets::SlopePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdSlopeSettings genSlope() const override;
    };

    struct NoReflection final : public Presets::ShaderPresets::SlopePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdSlopeSettings genSlope() const override;
    };

    struct Reflective final : public Presets::ShaderPresets::SlopePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdSlopeSettings genSlope() const override;
    };


    struct Translucent final : public Presets::ShaderPresets::SlopePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdSlopeSettings genSlope() const override;
    };
    struct Reversed final : public Presets::ShaderPresets::SlopePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdSlopeSettings genSlope() const override;
    };

    struct Micro final : public Presets::ShaderPresets::SlopePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdSlopeSettings genSlope() const override;
    };

    struct Nano final : public Presets::ShaderPresets::SlopePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] ShdSlopeSettings genSlope() const override;
    };
}
