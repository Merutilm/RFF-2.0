//
// Created by Merutilm on 9/4/26.
//

#pragma once
#include "../../Presets.hpp"

namespace merutilm::rff2::ClcCompressPresets {
    struct None final : public Presets::CalculationPresets::CompressPreset {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] FrtMPASettings genMPA() const override;
        [[nodiscard]] FrtReferenceCompSettings genRefComp() const override;
    };
    struct Stable final : public Presets::CalculationPresets::CompressPreset {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] FrtMPASettings genMPA() const override;
        [[nodiscard]] FrtReferenceCompSettings genRefComp() const override;
    };
    struct MoreStable final : public Presets::CalculationPresets::CompressPreset {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] FrtMPASettings genMPA() const override;
        [[nodiscard]] FrtReferenceCompSettings genRefComp() const override;
    };
    struct UltraStable final : public Presets::CalculationPresets::CompressPreset {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] FrtMPASettings genMPA() const override;
        [[nodiscard]] FrtReferenceCompSettings genRefComp() const override;
    };
}
