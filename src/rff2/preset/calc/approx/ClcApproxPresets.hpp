//
// Created by Merutilm on 2025-05-31.
//

#pragma once
#include "../../Presets.hpp"

namespace merutilm::rff2::ClcApproxPresets {
    struct UltraFast final : public Presets::CalculationPresets::ApproxPreset {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] FrtMPASettings genMPA() const override;
    };
    struct Fast final : public Presets::CalculationPresets::ApproxPreset {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] FrtMPASettings genMPA() const override;
    };
    struct Normal final : public Presets::CalculationPresets::ApproxPreset {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] FrtMPASettings genMPA() const override;
    };
    struct Best final : public Presets::CalculationPresets::ApproxPreset {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] FrtMPASettings genMPA() const override;
    };
    struct UltraBest final : public Presets::CalculationPresets::ApproxPreset {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] FrtMPASettings genMPA() const override;
    };
    struct LightSpirals final : public Presets::CalculationPresets::ApproxPreset {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] FrtMPASettings genMPA() const override;
    };
    struct DenseSpirals final : public Presets::CalculationPresets::ApproxPreset {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] FrtMPASettings genMPA() const override;
    };
    struct ExtremelyDenseSpirals final : public Presets::CalculationPresets::ApproxPreset {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] FrtMPASettings genMPA() const override;
    };
}
