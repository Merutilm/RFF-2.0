//
// Created by Merutilm on 2025-05-31.
//

#pragma once
#include "../Presets.h"
#include "../../settings/MPASettings.h"
#include "../../settings/ReferenceCompressionSettings.h"

namespace merutilm::rff2::CalculationPresets {
    struct UltraFast final : public Presets::CalculationPresets {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] MPASettings mpaSettings() const override;
        [[nodiscard]] ReferenceCompressionSettings referenceCompressionSettings() const override;
    };
    struct Fast final : public Presets::CalculationPresets {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] MPASettings mpaSettings() const override;
        [[nodiscard]] ReferenceCompressionSettings referenceCompressionSettings() const override;
    };
    struct Normal final : public Presets::CalculationPresets {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] MPASettings mpaSettings() const override;
        [[nodiscard]] ReferenceCompressionSettings referenceCompressionSettings() const override;
    };
    struct Best final : public Presets::CalculationPresets {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] MPASettings mpaSettings() const override;
        [[nodiscard]] ReferenceCompressionSettings referenceCompressionSettings() const override;
    };
    struct UltraBest final : public Presets::CalculationPresets {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] MPASettings mpaSettings() const override;
        [[nodiscard]] ReferenceCompressionSettings referenceCompressionSettings() const override;
    };
    struct Stable final : public Presets::CalculationPresets {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] MPASettings mpaSettings() const override;
        [[nodiscard]] ReferenceCompressionSettings referenceCompressionSettings() const override;
    };
    struct MoreStable final : public Presets::CalculationPresets {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] MPASettings mpaSettings() const override;
        [[nodiscard]] ReferenceCompressionSettings referenceCompressionSettings() const override;
    };
    struct UltraStable final : public Presets::CalculationPresets {
        [[nodiscard]] std::string getName() const override;
        [[nodiscard]] MPASettings mpaSettings() const override;
        [[nodiscard]] ReferenceCompressionSettings referenceCompressionSettings() const override;
    };
}
