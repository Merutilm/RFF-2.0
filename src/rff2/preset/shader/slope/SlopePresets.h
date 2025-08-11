//
// Created by Merutilm on 2025-05-28.
//

#pragma once
#include "../../Presets.h"
#include "../../../settings/SlopeSettings.h"


namespace merutilm::rff2::SlopePresets {

    struct Disabled final : public Presets::ShaderPresets::SlopePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] SlopeSettings slopeSettings() const override;
    };

    struct NoReflection final : public Presets::ShaderPresets::SlopePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] SlopeSettings slopeSettings() const override;
    };

    struct Reflective final : public Presets::ShaderPresets::SlopePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] SlopeSettings slopeSettings() const override;
    };


    struct Translucent final : public Presets::ShaderPresets::SlopePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] SlopeSettings slopeSettings() const override;
    };
    struct Reversed final : public Presets::ShaderPresets::SlopePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] SlopeSettings slopeSettings() const override;
    };

    struct Micro final : public Presets::ShaderPresets::SlopePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] SlopeSettings slopeSettings() const override;
    };

    struct Nano final : public Presets::ShaderPresets::SlopePreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] SlopeSettings slopeSettings() const override;
    };
}
