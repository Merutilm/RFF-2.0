//
// Created by Merutilm on 9/4/26.
//

#pragma once
#include "../../Presets.hpp"

namespace merutilm::rff2::ClcCompressPresets {
    struct None final : Presets::CalculationPresets::CompressPreset {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] FrtReferenceCompSettings genRefComp() const override;
    };
    struct SemiCompressed final : Presets::CalculationPresets::CompressPreset {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] FrtReferenceCompSettings genRefComp() const override;
    };
    struct Compressed final : Presets::CalculationPresets::CompressPreset {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] FrtReferenceCompSettings genRefComp() const override;
    };
    struct MoreCompressed final : Presets::CalculationPresets::CompressPreset {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] FrtReferenceCompSettings genRefComp() const override;
    };
}
