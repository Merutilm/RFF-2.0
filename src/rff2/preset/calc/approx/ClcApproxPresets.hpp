//
// Created by Merutilm on 2025-05-31.
//

#pragma once
#include "../../Presets.hpp"

namespace merutilm::rff2::ClcApproxPresets {
    struct UltraFast final : Presets::CalculationPresets::ApproxPreset {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] FrtMPASettings genMPA() const override;
    };
    struct Fast final : Presets::CalculationPresets::ApproxPreset {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] FrtMPASettings genMPA() const override;
    };
    struct Normal final : Presets::CalculationPresets::ApproxPreset {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] FrtMPASettings genMPA() const override;
    };
    struct Best final : Presets::CalculationPresets::ApproxPreset {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] FrtMPASettings genMPA() const override;
    };
    struct UltraBest final : Presets::CalculationPresets::ApproxPreset {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] FrtMPASettings genMPA() const override;
    };
}
