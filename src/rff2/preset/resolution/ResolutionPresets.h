//
// Created by Merutilm on 2025-05-31.
//

#pragma once
#include "../Presets.h"

namespace merutilm::rff2::ResolutionPresets {
    struct L1 final : public Presets::ResolutionPreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] std::array<int, 2> genResolution() const override;
    };

    struct L2 final : public Presets::ResolutionPreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] std::array<int, 2> genResolution() const override;
    };

    struct L3 final : public Presets::ResolutionPreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] std::array<int, 2> genResolution() const override;
    };

    struct L4 final : public Presets::ResolutionPreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] std::array<int, 2> genResolution() const override;
    };

    struct L5 final : public Presets::ResolutionPreset {
        [[nodiscard]] std::string getName() const override;

        [[nodiscard]] std::array<int, 2> genResolution() const override;
    };
}
